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

#include "ovf-parser.h"

#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

struct _OvfParser
{
	GObject parent_instance;
};

G_DEFINE_TYPE (OvfParser, ovf_parser, G_TYPE_OBJECT)

G_DEFINE_QUARK (ovf-parser-error-quark, ovf_parser_error)

#define OVF_PATH_ENVELOPE "/ovf:Envelope[1]"
#define OVF_PATH_VIRTUALSYSTEM OVF_PATH_ENVELOPE "/ovf:VirtualSystem"
#define OVF_PATH_OPERATINGSYSTEM OVF_PATH_VIRTUALSYSTEM "/ovf:OperatingSystemSection"
#define OVF_PATH_VIRTUALHARDWARE OVF_PATH_VIRTUALSYSTEM "/ovf:VirtualHardwareSection"

gboolean
ovf_parser_load_from_file (OvfParser     *self,
                           const gchar   *filename,
                           GError       **error)
{
	g_autofree gchar *data = NULL;
	gsize length;

	g_return_val_if_fail (OVF_IS_PARSER (self), FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);

	if (!g_file_get_contents (filename, &data, &length, error))
		return FALSE;

	return ovf_parser_load_from_data (self, data, length, error);
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

gboolean
ovf_parser_load_from_data (OvfParser    *self,
                           const gchar  *data,
                           gssize        length,
                           GError      **error)
{
	gboolean ret;
	xmlDoc *doc = NULL;
	xmlXPathContext *ctx = NULL;

	g_return_val_if_fail (OVF_IS_PARSER (self), FALSE);
	g_return_val_if_fail (data != NULL, FALSE);

	doc = xmlParseMemory (data, length);
	if (doc == NULL) {
		g_set_error (error,
		             OVF_PARSER_ERROR,
		             OVF_PARSER_ERROR_XML,
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
		             OVF_PARSER_ERROR,
		             OVF_PARSER_ERROR_XML,
		             "Could not find VirtualSystem section");
		ret = FALSE;
		goto out;
	}

	if (!xpath_section_exists (ctx, OVF_PATH_OPERATINGSYSTEM)) {
		g_set_error (error,
		             OVF_PARSER_ERROR,
		             OVF_PARSER_ERROR_XML,
		             "Could not find OperatingSystem section");
		ret = FALSE;
		goto out;
	}

	if (!xpath_section_exists (ctx, OVF_PATH_VIRTUALHARDWARE)) {
		g_set_error (error,
		             OVF_PARSER_ERROR,
		             OVF_PARSER_ERROR_XML,
		             "Could not find VirtualHardware section");
		ret = FALSE;
		goto out;
	}

	ret = TRUE;
out:
	if (doc != NULL)
		xmlFreeDoc (doc);
	if (ctx != NULL)
		xmlXPathFreeContext (ctx);

	return ret;
}

OvfParser *
ovf_parser_new (void)
{
	return g_object_new (OVF_TYPE_PARSER, NULL);
}

static void
ovf_parser_class_init (OvfParserClass *klass)
{
}

static void
ovf_parser_init (OvfParser *self)
{
}
