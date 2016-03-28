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

#ifndef __OVF_PARSER_H__
#define __OVF_PARSER_H__

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OVF_TYPE_PARSER (ovf_parser_get_type ())
G_DECLARE_FINAL_TYPE (OvfParser, ovf_parser, OVF, PARSER, GObject)

#define OVF_PARSER_ERROR (ovf_parser_error_quark ())

typedef enum
{
	OVF_PARSER_ERROR_XML,
	OVF_PARSER_ERROR_LAST
} OvfParserError;

GQuark			  ovf_parser_error_quark		(void);

OvfParser		 *ovf_parser_new			(void);
gboolean		  ovf_parser_load_from_file		(OvfParser		 *self,
								 const gchar		 *filename,
								 GError			**error);


G_END_DECLS

#endif /* __OVF_PARSER_H__ */
