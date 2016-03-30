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

#ifndef __OVF_PACKAGE_H__
#define __OVF_PACKAGE_H__

#include <gio/gio.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OVF_TYPE_PACKAGE (ovf_package_get_type ())
G_DECLARE_FINAL_TYPE (OvfPackage, ovf_package, OVF, PACKAGE, GObject)

#define OVF_PACKAGE_ERROR (ovf_package_error_quark ())

typedef enum
{
	OVF_PACKAGE_ERROR_XML,
	OVF_PACKAGE_ERROR_LAST
} OvfPackageError;

GQuark			  ovf_package_error_quark		(void);

OvfPackage		 *ovf_package_new			(void);
gboolean		  ovf_package_load_from_file		(OvfPackage		 *self,
								 const gchar		 *filename,
								 GError			**error);
gboolean		  ovf_package_load_from_data		(OvfPackage		 *self,
								 const gchar		 *data,
								 gssize			  length,
								 GError			**error);



G_END_DECLS

#endif /* __OVF_PACKAGE_H__ */
