/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2017 Joaquim Rocha <jrocha@endlessm.com>
 *
 * Licensed under the GNU General Public License Version 2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"

#include "gnome-software-private.h"

#include "gs-appstream.h"
#include "gs-test.h"

static void
gs_plugins_core_app_creation_func (GsPluginLoader *plugin_loader)
{
	AsApp *as_app = NULL;
	GsApp *cached_app = NULL;
	GsPlugin *plugin;
	gboolean ret;
	g_autoptr(GsApp) app = NULL;
	g_autoptr(AsStore) store = NULL;
	g_autoptr(GError) error = NULL;
	const gchar *test_icon_root = g_getenv ("GS_SELF_TEST_APPSTREAM_ICON_ROOT");
	g_autofree gchar *xml = g_strdup ("<?xml version=\"1.0\"?>\n"
					  "<components version=\"0.9\">\n"
					  "  <component type=\"desktop\">\n"
					  "    <id>demeter.desktop</id>\n"
					  "    <name>Demeter</name>\n"
					  "    <summary>An agriculture application</summary>\n"
					  "  </component>\n"
					  "</components>\n");

	/* drop all caches */
	gs_plugin_loader_setup_again (plugin_loader);

	app = gs_plugin_loader_app_create (plugin_loader,
					   "*/*/*/desktop/demeter.desktop/*");
	gs_app_add_quirk (app, AS_APP_QUIRK_MATCH_ANY_PREFIX);

	cached_app = gs_plugin_loader_app_create (plugin_loader,
						  "*/*/*/desktop/demeter.desktop/*");

	g_assert (app == cached_app);

	/* Make sure the app still has the match-any-prefix quirk*/
	g_assert(gs_app_has_quirk (app, AS_APP_QUIRK_MATCH_ANY_PREFIX));

	/* Ensure gs_appstream creates a new app when a matching one is cached but
	 * has the match-any-prefix quirk */
	store = as_store_new ();
	ret = as_store_from_xml (store, xml, test_icon_root, &error);
	g_assert_no_error (error);
	g_assert (ret);

	as_app = as_store_get_app_by_id (store, "demeter.desktop");
	g_assert (as_app != NULL);

	plugin = gs_plugin_loader_find_plugin (plugin_loader, "appstream");
	g_assert (plugin != NULL);

	g_clear_object (&app);
	app = gs_appstream_create_app (plugin, as_app, NULL);
	g_assert (app != NULL);
	g_assert (cached_app != app);
	g_assert (!gs_app_has_quirk (app, AS_APP_QUIRK_MATCH_ANY_PREFIX));

	g_clear_object (&cached_app);
	cached_app = gs_plugin_loader_app_create (plugin_loader,
						  "*/*/*/desktop/demeter.desktop/*");
	g_assert (cached_app == app);
}

int
main (int argc, char **argv)
{
	const gchar *tmp_root = "/var/tmp/self-test";
	gboolean ret;
	g_autofree gchar *xml = NULL;
	g_autoptr(GError) error = NULL;
	g_autoptr(GsPluginLoader) plugin_loader = NULL;
	const gchar *whitelist[] = {
		"appstream",
		"icons",
		NULL
	};

	g_test_init (&argc, &argv, NULL);
	g_setenv ("G_MESSAGES_DEBUG", "all", TRUE);
	g_setenv ("GS_SELF_TEST_CORE_DATADIR", tmp_root, TRUE);

	/* ensure test root does not exist */
	if (g_file_test (tmp_root, G_FILE_TEST_EXISTS)) {
		ret = gs_utils_rmtree (tmp_root, &error);
		g_assert_no_error (error);
		g_assert (ret);
		g_assert (!g_file_test (tmp_root, G_FILE_TEST_EXISTS));
	}

	//g_setenv ("GS_SELF_TEST_APPSTREAM_XML", xml, TRUE);
	g_setenv ("GS_SELF_TEST_APPSTREAM_ICON_ROOT",
		  "/var/tmp/self-test/flatpak/appstream/test/x86_64/active/", TRUE);

	/* only critical and error are fatal */
	g_log_set_fatal_mask (NULL, G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL);

	/* we can only load this once per process */
	plugin_loader = gs_plugin_loader_new ();
	gs_plugin_loader_add_location (plugin_loader, LOCALPLUGINDIR);
	gs_plugin_loader_add_location (plugin_loader, LOCALPLUGINDIR_CORE);
	ret = gs_plugin_loader_setup (plugin_loader,
				      (gchar**) whitelist,
				      NULL,
				      GS_PLUGIN_FAILURE_FLAGS_NONE,
				      NULL,
				      &error);
	g_assert_no_error (error);
	g_assert (ret);

	/* plugin tests go here */
	g_test_add_data_func ("/gnome-software/plugins/core/app-creation",
			      plugin_loader,
			      (GTestDataFunc) gs_plugins_core_app_creation_func);
	return g_test_run ();
}

/* vim: set noexpandtab: */
