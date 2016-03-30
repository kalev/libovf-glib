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

#include <libovf-glib/ovf-package.h>

static void
test_init_parser (void)
{
	g_autoptr(OvfPackage) ovf_package = NULL;

	ovf_package = ovf_package_new ();
	g_assert (OVF_IS_PACKAGE (ovf_package));
}

static void
test_missing_sections (void)
{
	g_autoptr(GError) error = NULL;
	g_autoptr(OvfPackage) ovf_package = NULL;
	gchar data[] =
"<?xml version=\"1.0\"?>"
"<Envelope ovf:version=\"1.0\" xml:lang=\"en-US\" xmlns=\"http://schemas.dmtf.org/ovf/envelope/1\" xmlns:ovf=\"http://schemas.dmtf.org/ovf/envelope/1\">"
"  <VirtualSystem ovf:id=\"Fedora 23\">"
"  </VirtualSystem>"
"</Envelope>";

	ovf_package = ovf_package_new ();
	ovf_package_load_from_data (ovf_package, data, -1, &error);
	g_assert_error (error, OVF_PACKAGE_ERROR, OVF_PACKAGE_ERROR_XML);
}

static void
test_load_valid_ovf (void)
{
	g_autoptr(GError) error = NULL;
	g_autoptr(OvfPackage) ovf_package = NULL;

	ovf_package = ovf_package_new ();
	ovf_package_load_from_file (ovf_package,
	                            g_test_get_filename (G_TEST_DIST, "Fedora_23.ovf", NULL),
	                            &error);
	g_assert_no_error (error);
}

static void
test_load_valid_ova (void)
{
	g_autoptr(GError) error = NULL;
	g_autoptr(OvfPackage) ovf_package = NULL;

	ovf_package = ovf_package_new ();
	ovf_package_load_from_ova_file (ovf_package,
	                                g_test_get_filename (G_TEST_DIST, "Fedora_23.ova", NULL),
	                                &error);
	g_assert_no_error (error);
}

static void
test_save_ovf (void)
{
	g_autofree gchar *contents = NULL;
	g_autoptr(GError) error = NULL;
	g_autoptr(OvfPackage) ovf_package = NULL;
	gsize length = 0;

	/* first load in an ovf file */
	ovf_package = ovf_package_new ();
	ovf_package_load_from_file (ovf_package,
	                            g_test_get_filename (G_TEST_DIST, "Fedora_23.ovf", NULL),
	                            &error);
	g_assert_no_error (error);

	/* save it back to disk */
	ovf_package_save_file (ovf_package, "/tmp/Fedora_23.ovf", &error);
	g_assert_no_error (error);

	/* make sure it's not 0 bytes */
	g_file_get_contents ("/tmp/Fedora_23.ovf", &contents, &length, &error);
	g_assert (length > 0);
	g_assert_no_error (error);
}

int
main (int   argc,
      char *argv[])
{
	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/parser/init-parser", test_init_parser);
	g_test_add_func ("/parser/missing-sections", test_missing_sections);
	g_test_add_func ("/parser/load-valid-ovf", test_load_valid_ovf);
	g_test_add_func ("/parser/load-valid-ova", test_load_valid_ova);
	g_test_add_func ("/parser/save-ovf", test_save_ovf);

	return g_test_run ();
}
