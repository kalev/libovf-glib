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

gboolean
ovf_parser_load_from_file (OvfParser     *self,
                           const gchar   *filename,
                           GError       **error)
{
	gboolean ret;
	xmlDoc *doc = NULL;
	xmlXPathContext *ctx = NULL;

	g_debug ("loading from file '%s'", filename);

	doc = xmlParseDoc ((const xmlChar *) filename);
	if (doc == NULL) {
		g_set_error (error,
		             OVF_PARSER_ERROR,
		             OVF_PARSER_ERROR_XML,
		             "Could not parse '%s'",
		             filename);

		ret = FALSE;
		goto out;
	}

	ctx = xmlXPathNewContext (doc);
	xmlXPathRegisterNs (ctx,
	                    (const xmlChar *) "ovf",
	                    (const xmlChar *) "http://schemas.dmtf.org/ovf/envelope/1");

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
