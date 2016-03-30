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

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

struct _OvfPackage
{
	GObject parent_instance;
};

G_DEFINE_TYPE (OvfPackage, ovf_package, G_TYPE_OBJECT)

G_DEFINE_QUARK (ovf-package-error-quark, ovf_package_error)

#define OVF_PATH_ENVELOPE "/ovf:Envelope[1]"
#define OVF_PATH_VIRTUALSYSTEM OVF_PATH_ENVELOPE "/ovf:VirtualSystem"
#define OVF_PATH_OPERATINGSYSTEM OVF_PATH_VIRTUALSYSTEM "/ovf:OperatingSystemSection"
#define OVF_PATH_VIRTUALHARDWARE OVF_PATH_VIRTUALSYSTEM "/ovf:VirtualHardwareSection"

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

static gboolean
xpath_section_exists (xmlXPathContext *ctx, const gchar *path)
{
	xmlXPathObject *obj;

	obj = xmlXPathEval ((const xmlChar *) path, ctx);
	if (obj == NULL ||
	    obj->type != XPATH_NODESET ||
	    obj->nodesetval == NULL ||
	    obj->nodesetval->nodeNr == 0) {
		return FALSE;
	}

	xmlXPathFreeObject (obj);
	return TRUE;
}

static gchar *
xpath_str (xmlXPathContext *ctx, const gchar *path)
{
	xmlChar *content;
	xmlXPathObject *obj;
	gchar *ret;

	obj = xmlXPathEval ((const xmlChar *) path, ctx);
	if (obj == NULL ||
	    obj->type != XPATH_NODESET ||
	    obj->nodesetval == NULL ||
	    obj->nodesetval->nodeNr == 0) {
		return NULL;
	}

	content = xmlNodeGetContent (obj->nodesetval->nodeTab[0]);
	ret = g_strdup ((const gchar *) content);

	xmlFree (content);
	xmlXPathFreeObject (obj);
	return ret;
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
	xmlDoc *doc = NULL;
	xmlXPathContext *ctx = NULL;

	g_return_val_if_fail (OVF_IS_PACKAGE (self), FALSE);
	g_return_val_if_fail (data != NULL, FALSE);

	doc = xmlParseMemory (data, length);
	if (doc == NULL) {
		g_set_error (error,
		             OVF_PACKAGE_ERROR,
		             OVF_PACKAGE_ERROR_XML,
		             "Could not parse XML");
		ret = FALSE;
		goto out;
	}

	ctx = xmlXPathNewContext (doc);
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

	ret = TRUE;
out:
	if (doc != NULL)
		xmlFreeDoc (doc);
	if (ctx != NULL)
		xmlXPathFreeContext (ctx);

	return ret;
}

OvfPackage *
ovf_package_new (void)
{
	return g_object_new (OVF_TYPE_PACKAGE, NULL);
}

static void
ovf_package_class_init (OvfPackageClass *klass)
{
}

static void
ovf_package_init (OvfPackage *self)
{
}
