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

#include "ovf-disk.h"

struct _OvfDisk
{
	GObject			  parent_instance;

	gchar			 *capacity;
	gchar			 *disk_id;
	gchar			 *file_ref;
	gchar			 *format;
};

G_DEFINE_TYPE (OvfDisk, ovf_disk, G_TYPE_OBJECT)

const gchar *
ovf_disk_get_capacity (OvfDisk *self)
{
	return self->capacity;
}

void
ovf_disk_set_capacity (OvfDisk     *self,
                       const gchar *capacity)
{
	g_free (self->capacity);
	self->capacity = g_strdup (capacity);
}

const gchar *
ovf_disk_get_disk_id (OvfDisk *self)
{
	return self->disk_id;
}

void
ovf_disk_set_disk_id (OvfDisk     *self,
                      const gchar *disk_id)
{
	g_free (self->disk_id);
	self->disk_id = g_strdup (disk_id);
}

const gchar *
ovf_disk_get_file_ref (OvfDisk *self)
{
	return self->file_ref;
}

void
ovf_disk_set_file_ref (OvfDisk     *self,
                       const gchar *file_ref)
{
	g_free (self->file_ref);
	self->file_ref = g_strdup (file_ref);
}

const gchar *
ovf_disk_get_format (OvfDisk *self)
{
	return self->format;
}

void
ovf_disk_set_format (OvfDisk     *self,
                     const gchar *format)
{
	g_free (self->format);
	self->format = g_strdup (format);
}

OvfDisk *
ovf_disk_new (void)
{
	return g_object_new (OVF_TYPE_DISK, NULL);
}

static void
ovf_disk_finalize (GObject *object)
{
	OvfDisk *self = OVF_DISK (object);

	g_free (self->capacity);
	g_free (self->disk_id);
	g_free (self->file_ref);
	g_free (self->format);

	G_OBJECT_CLASS (ovf_disk_parent_class)->finalize (object);
}

static void
ovf_disk_class_init (OvfDiskClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = ovf_disk_finalize;
}

static void
ovf_disk_init (OvfDisk *self)
{
}
