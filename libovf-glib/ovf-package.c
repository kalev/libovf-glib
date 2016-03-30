/*
 * Copyright (C) 2016 Kalev Lember <klember@redhat.com>
 *
 * Licensed under the GNU Lesser General Public License Version 2.1
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "ovf-package.h"

#include "ovf-disk.h"

#include <archive.h>
#include <archive_entry.h>
#include <glib/gstdio.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <string.h>

struct _OvfPackage
{
	GObject			  parent_instance;

	GPtrArray		 *disks;
	xmlDoc			 *doc;
};

G_DEFINE_TYPE (OvfPackage, ovf_package, G_TYPE_OBJECT)

G_DEFINE_QUARK (ovf-package-error-quark, ovf_package_error)

#define OVF_NS_ENVELOPE "http://schemas.dmtf.org/ovf/envelope/1"

#define OVF_PATH_ENVELOPE "/ovf:Envelope[1]"
#define OVF_PATH_DISKSECTION OVF_PATH_ENVELOPE "/ovf:DiskSection"
#define OVF_PATH_VIRTUALSYSTEM OVF_PATH_ENVELOPE "/ovf:VirtualSystem"
#define OVF_PATH_OPERATINGSYSTEM OVF_PATH_VIRTUALSYSTEM "/ovf:OperatingSystemSection"
#define OVF_PATH_VIRTUALHARDWARE OVF_PATH_VIRTUALSYSTEM "/ovf:VirtualHardwareSection"

static gboolean
endswith (const gchar *str, const gchar *search)
{
	gsize str_len = strlen (str);
	gsize search_len = strlen (search);

	if (str_len < search_len)
		return FALSE;

	return g_ascii_strcasecmp (str + str_len - search_len, search) == 0;
}

static gboolean
extract_ovf_to_fd (const gchar  *ova_filename,
                   gint          out_fd,
                   GError      **error)
{
	gboolean found;
	gboolean ret = TRUE;
	int r;
	struct archive *a = NULL;

	/* open the .ova archive */
	a = archive_read_new ();
	archive_read_support_format_all (a);
	archive_read_support_filter_all (a);
	r = archive_read_open_filename (a, ova_filename, 10240);
	if (r != ARCHIVE_OK) {
		g_set_error (error,
			     OVF_PACKAGE_ERROR,
			     OVF_PACKAGE_ERROR_FAILED,
			     "Cannot open: %s",
			     archive_error_string (a));
		ret = FALSE;
		goto out;
	}

	/* find first .ovf file and extract it */
	found = FALSE;
	for (;;) {
		g_autofree gchar *buf = NULL;
		const gchar *name;
		struct archive_entry *entry;

		r = archive_read_next_header (a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r != ARCHIVE_OK) {
			g_set_error (error,
			             OVF_PACKAGE_ERROR,
			             OVF_PACKAGE_ERROR_FAILED,
			             "Cannot read header: %s",
			             archive_error_string (a));
			ret = FALSE;
			goto out;
		}

		/* did we find an .ovf file? */
		name = archive_entry_pathname (entry);
		if (name != NULL && endswith (name, ".ovf")) {
			r = archive_read_data_into_fd (a, out_fd);
			if (r != ARCHIVE_OK) {
				g_set_error (error,
				             OVF_PACKAGE_ERROR,
				             OVF_PACKAGE_ERROR_FAILED,
				             "Cannot extract: %s",
				             archive_error_string (a));
				ret = FALSE;
				goto out;
			}
			found = TRUE;
			break;
		}
	}

	if (!found) {
		g_set_error (error,
		             OVF_PACKAGE_ERROR,
		             OVF_PACKAGE_ERROR_NOT_FOUND,
		             "Could not find any .ovf files");
		ret = FALSE;
	}

out:
	if (a != NULL) {
		archive_read_close (a);
		archive_read_free (a);
	}
	return ret;
}

gboolean
ovf_package_load_from_ova_file (OvfPackage   *self,
                                const gchar  *filename,
                                GError      **error)
{
	g_autofree gchar *tmp_path = NULL;
	gint tmp_fd = -1;
	gboolean ret = TRUE;

	g_return_val_if_fail (OVF_IS_PACKAGE (self), FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);

	/* open a temporary file */
	tmp_fd = g_file_open_tmp ("ovf-package-XXXXXX.ovf", &tmp_path, error);
	if (tmp_fd == -1) {
		ret = FALSE;
		goto out;
	}

	/* extract an .ovf file */
	if (!extract_ovf_to_fd (filename, tmp_fd, error)) {
		ret = FALSE;
		goto out;
	}

	if (!ovf_package_load_from_file (self, tmp_path, error)) {
		ret = FALSE;
		goto out;
	}

out:
	if (tmp_fd != -1) {
		close (tmp_fd);
		g_unlink (tmp_path);
	}

	return ret;
}

gboolean
ovf_package_load_from_file (OvfPackage   *self,
                            const gchar  *filename,
                            GError      **error)
{
	g_autofree gchar *data = NULL;
	gsize length;

	g_return_val_if_fail (OVF_IS_PACKAGE (self), FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);

	if (!g_file_get_contents (filename, &data, &length, error))
		return FALSE;

	return ovf_package_load_from_data (self, data, length, error);
}

gboolean
ovf_package_save_file (OvfPackage   *self,
                       const gchar  *filename,
                       GError      **error)
{
	gboolean ret = TRUE;
	int length;
	xmlChar *data = NULL;

	g_return_val_if_fail (OVF_IS_PACKAGE (self), FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);

	xmlDocDumpMemory (self->doc, &data, &length);

	if (!g_file_set_contents (filename, (const gchar *) data, (gsize) length, error)) {
		ret = FALSE;
		goto out;
	}

out:
	if (data != NULL)
		xmlFree (data);

	return ret;
}

static gboolean
xpath_section_exists (xmlXPathContext *ctx, const gchar *path)
{
	gboolean ret = TRUE;
	xmlXPathObject *obj;

	obj = xmlXPathEval ((const xmlChar *) path, ctx);
	if (obj == NULL ||
	    obj->type != XPATH_NODESET ||
	    obj->nodesetval == NULL ||
	    obj->nodesetval->nodeNr == 0) {
		ret = FALSE;
		goto out;
	}

out:
	if (obj != NULL)
		xmlXPathFreeObject (obj);

	return ret;
}

static gchar *
xpath_str (xmlXPathContext *ctx, const gchar *path)
{
	xmlChar *content = NULL;
	xmlXPathObject *obj;
	gchar *ret;

	obj = xmlXPathEval ((const xmlChar *) path, ctx);
	if (obj == NULL ||
	    obj->type != XPATH_NODESET ||
	    obj->nodesetval == NULL ||
	    obj->nodesetval->nodeNr == 0) {
		ret = NULL;
		goto out;
	}

	content = xmlNodeGetContent (obj->nodesetval->nodeTab[0]);
	ret = g_strdup ((const gchar *) content);

out:
	if (content != NULL)
		xmlFree (content);
	if (obj != NULL)
		xmlXPathFreeObject (obj);

	return ret;
}

static GPtrArray *
parse_disks (xmlXPathContext *ctx)
{
	gint i;
	g_autoptr(GPtrArray) disks = NULL;
	xmlXPathObject *obj;

	obj = xmlXPathEval ((const xmlChar *) OVF_PATH_DISKSECTION "/ovf:Disk", ctx);
	if (obj == NULL ||
	    obj->type != XPATH_NODESET ||
	    obj->nodesetval == NULL ||
	    obj->nodesetval->nodeNr == 0) {
		goto out;
	}

	disks = g_ptr_array_new_with_free_func (g_object_unref);
	for (i = 0; i < obj->nodesetval->nodeNr; i++) {
		OvfDisk *disk = ovf_disk_new ();
		xmlNode *node = obj->nodesetval->nodeTab[i];
		xmlChar *str;

		str = xmlGetNsProp (node,
		                    (const xmlChar *) "capacity",
		                    (const xmlChar *) OVF_NS_ENVELOPE);
		ovf_disk_set_capacity (disk, (const gchar *) str);
		xmlFree (str);

		str = xmlGetNsProp (node,
		                    (const xmlChar *) "diskId",
		                    (const xmlChar *) OVF_NS_ENVELOPE);
		ovf_disk_set_disk_id (disk, (const gchar *) str);
		xmlFree (str);

		str = xmlGetNsProp (node,
		                    (const xmlChar *) "fileRef",
		                    (const xmlChar *) OVF_NS_ENVELOPE);
		ovf_disk_set_file_ref (disk, (const gchar *) str);
		xmlFree (str);

		str = xmlGetNsProp (node,
		                    (const xmlChar *) "format",
		                    (const xmlChar *) OVF_NS_ENVELOPE);
		ovf_disk_set_format (disk, (const gchar *) str);
		xmlFree (str);

		g_ptr_array_add (disks, disk);
	}

out:
	if (obj != NULL)
		xmlXPathFreeObject (obj);

	return g_steal_pointer (&disks);
}
gboolean
ovf_package_load_from_data (OvfPackage   *self,
                            const gchar  *data,
                            gssize        length,
                            GError      **error)
{
	g_autofree gchar *desc = NULL;
	g_autofree gchar *name = NULL;
	gboolean ret;
	xmlXPathContext *ctx = NULL;

	g_return_val_if_fail (OVF_IS_PACKAGE (self), FALSE);
	g_return_val_if_fail (data != NULL, FALSE);

	if (self->doc != NULL)
		xmlFreeDoc (self->doc);

	self->doc = xmlParseMemory (data, length);
	if (self->doc == NULL) {
		g_set_error (error,
		             OVF_PACKAGE_ERROR,
		             OVF_PACKAGE_ERROR_XML,
		             "Could not parse XML");
		ret = FALSE;
		goto out;
	}

	ctx = xmlXPathNewContext (self->doc);
	xmlXPathRegisterNs (ctx,
	                    (const xmlChar *) "ovf",
	                    (const xmlChar *) "http://schemas.dmtf.org/ovf/envelope/1");

	if (!xpath_section_exists (ctx, OVF_PATH_VIRTUALSYSTEM)) {
		g_set_error (error,
		             OVF_PACKAGE_ERROR,
		             OVF_PACKAGE_ERROR_XML,
		             "Could not find VirtualSystem section");
		ret = FALSE;
		goto out;
	}

	if (!xpath_section_exists (ctx, OVF_PATH_OPERATINGSYSTEM)) {
		g_set_error (error,
		             OVF_PACKAGE_ERROR,
		             OVF_PACKAGE_ERROR_XML,
		             "Could not find OperatingSystem section");
		ret = FALSE;
		goto out;
	}

	if (!xpath_section_exists (ctx, OVF_PATH_VIRTUALHARDWARE)) {
		g_set_error (error,
		             OVF_PACKAGE_ERROR,
		             OVF_PACKAGE_ERROR_XML,
		             "Could not find VirtualHardware section");
		ret = FALSE;
		goto out;
	}

	name = xpath_str (ctx, OVF_PATH_VIRTUALSYSTEM "/ovf:Name");
	desc = xpath_str (ctx, OVF_PATH_VIRTUALSYSTEM "/ovf:AnnotationSection/ovf:Annotation");

	g_debug ("name: %s, desc: %s", name, desc);

	if (self->disks != NULL)
		g_ptr_array_free (self->disks, TRUE);

	self->disks = parse_disks (ctx);

	ret = TRUE;
out:
	if (ctx != NULL)
		xmlXPathFreeContext (ctx);

	return ret;
}

GPtrArray *
ovf_package_get_disks (OvfPackage  *self)
{
	if (self->disks == NULL)
		return NULL;

	return g_ptr_array_ref (self->disks);
}

OvfPackage *
ovf_package_new (void)
{
	return g_object_new (OVF_TYPE_PACKAGE, NULL);
}

static void
ovf_package_finalize (GObject *object)
{
	OvfPackage *self = OVF_PACKAGE (object);

	if (self->disks != NULL)
		g_ptr_array_free (self->disks, TRUE);
	if (self->doc != NULL)
		xmlFreeDoc (self->doc);

	G_OBJECT_CLASS (ovf_package_parent_class)->finalize (object);
}

static void
ovf_package_class_init (OvfPackageClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = ovf_package_finalize;
}

static void
ovf_package_init (OvfPackage *self)
{
}
