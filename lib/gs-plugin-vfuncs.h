/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 * vi:set noexpandtab tabstop=8 shiftwidth=8:
 *
 * Copyright (C) 2012-2017 Richard Hughes <richard@hughsie.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#pragma once

/**
 * SECTION:gs-plugin-vfuncs
 * @title: GsPlugin Exports
 * @include: gnome-software.h
 * @stability: Unstable
 * @short_description: Vfuncs that plugins can implement
 */

#include <appstream.h>
#include <glib-object.h>
#include <gmodule.h>
#include <gio/gio.h>
#include <libsoup/soup.h>

#include "gs-app.h"
#include "gs-app-list.h"
#include "gs-category.h"

G_BEGIN_DECLS

/**
 * gs_plugin_query_type:
 *
 * Returns the #GType for a subclass of #GsPlugin provided by this plugin
 * module. It should not do any other computation.
 *
 * The init function for that type should initialize the plugin. If the plugin
 * should not be run then gs_plugin_set_enabled() should be called from the
 * init function.
 *
 * NOTE: Do not do any failable actions in the plugin class’ init function; use
 * #GsPluginClass.setup_async instead.
 *
 * Since: 42
 */
GType		 gs_plugin_query_type			(void);

/**
 * gs_plugin_adopt_app:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 *
 * Called when an #GsApp has not been claimed (i.e. a management plugin has not
 * been set).
 *
 * A claimed application means other plugins will not try to perform actions
 * such as install, remove or update. Most applications are claimed when they
 * are created.
 *
 * If a plugin can adopt this application then it should call
 * gs_app_set_management_plugin() on @app.
 **/
void		 gs_plugin_adopt_app			(GsPlugin	*plugin,
							 GsApp		*app);

/**
 * gs_plugin_add_search:
 * @plugin: a #GsPlugin
 * @values: a NULL terminated list of search terms, e.g. [ "gnome", "software" ]
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get search results for a specific query.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_search			(GsPlugin	*plugin,
							 gchar		**values,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_search_files:
 * @plugin: a #GsPlugin
 * @values: a list of filenames, e.g. [ "/usr/share/help/gimp/index.html" ]
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Called when searching for an application that provides a specific filename
 * on the filesystem.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_search_files		(GsPlugin	*plugin,
							 gchar		**values,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_search_what_provides
 * @plugin: a list of tags, e.g. [ "text/rtf" ]
 * @values: a #GStrv
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Called when searching for an application that provides specific defined tags,
 * for instance a codec string or mime-type.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_search_what_provides	(GsPlugin	*plugin,
							 gchar		**values,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_alternates
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Called when trying to find alternates to a specific app, for instance
 * finding a flatpak version of an existing distro packaged application.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_alternates		(GsPlugin	*plugin,
							 GsApp		*app,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_updates:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get the list of updates.
 *
 * NOTE: Actually downloading the updates can be done in gs_plugin_download_app()
 * or in gs_plugin_download().
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_updates			(GsPlugin	*plugin,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_sources:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get the list of sources, for example the repos listed in /etc/yum.repos.d
 * or the remotes configured in flatpak.
 *
 * Plugins are expected to add new apps using gs_app_list_add() of type
 * %AS_COMPONENT_KIND_REPOSITORY.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_sources			(GsPlugin	*plugin,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_updates_historical
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get the list of historical updates, i.e. the updates that have just been
 * installed.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_updates_historical	(GsPlugin	*plugin,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_categories:
 * @plugin: a #GsPlugin
 * @list: (element-type GsCategory): a #GPtrArray
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get the category tree, for instance Games->Action or Internet->Email.
 *
 * Plugins are expected to add new categories using g_ptr_array_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_categories		(GsPlugin	*plugin,
							 GPtrArray	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_category_apps:
 * @plugin: a #GsPlugin
 * @category: a #GsCategory
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get all the applications that match a specific category.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_category_apps		(GsPlugin	*plugin,
							 GsCategory	*category,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_recent:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @age: a number of seconds
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Return all the applications that have had upstream releases recently.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_recent			(GsPlugin	*plugin,
							 GsAppList	*list,
							 guint64	 age,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_popular:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get popular applications that should be featured on the main page as
 * "Editors Picks".
 * This is expected to be a curated list of applications that are high quality
 * and feature-complete.
 *
 * The returned list of popular applications are not sorted, but each #GsApp has
 * to be valid, for instance having a known state and a valid icon.
 * If an insufficient number of applications are added by plugins then the
 * section on the overview shell may be hidden.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_popular			(GsPlugin	*plugin,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_featured:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Get applications that should be featured as a large full-width banner on the
 * overview page.
 * This is expected to be a curated list of applications that are high quality
 * and feature-complete.
 *
 * The returned list of popular applications are randomized in a way so that
 * the same application is featured for the entire calendar day.
 *
 * NOTE: The UI code may expect that applications have additional metadata set on
 * results, for instance <code>GnomeSoftware::FeatureTile</code>.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_featured			(GsPlugin	*plugin,
							 GsAppList	*list,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_launch:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Launch the specified application using a plugin-specific method.
 * This is normally setting some environment or launching a specific binary.
 *
 * Plugins can simply use gs_plugin_app_launch() if no plugin-specific
 * functionality is required.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_launch			(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_update_cancel:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Cancels the offline update of @app.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_update_cancel		(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_app_install:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Install the application.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @app.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @app
 * to %GS_APP_STATE_INSTALLED.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_app_install			(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_app_remove:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Remove the application.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @app.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @app
 * to %GS_APP_STATE_AVAILABLE or %GS_APP_STATE_UNKNOWN if not known.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_app_remove			(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_update_app:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Update the application live.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @app.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @app
 * to %GS_APP_STATE_INSTALLED or %GS_APP_STATE_UNKNOWN if not known.
 *
 * If %GS_APP_QUIRK_IS_PROXY is set on the application then the actual #GsApp
 * set in @app will be the related application of the parent. Plugins do not
 * need to manually iterate on the related list of applications.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_update_app			(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_download_app:
 * @plugin: a #GsPlugin
 * @app: a #GsApp
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Downloads the application and any dependencies ready to be installed or
 * updated.
 *
 * Plugins are expected to schedule downloads using the system download
 * scheduler if appropriate (if the download is not guaranteed to be under a few
 * hundred kilobytes, for example), so that the user’s metered data preferences
 * are honoured.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @app.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * If the @app is already downloaded, do not return an error and return %TRUE.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_download_app			(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_download:
 * @plugin: a #GsPlugin
 * @apps: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Downloads a list of applications ready to be installed or updated.
 *
 * Plugins are expected to schedule downloads using the system download
 * scheduler if appropriate (if the download is not guaranteed to be under a few
 * hundred kilobytes, for example), so that the user’s metered data preferences
 * are honoured.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_download			(GsPlugin	*plugin,
							 GsAppList	*apps,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_app_upgrade_download:
 * @plugin: a #GsPlugin
 * @app: a #GsApp, with kind %AS_COMPONENT_KIND_OPERATING_SYSTEM
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Starts downloading a distribution upgrade in the background.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_app_upgrade_download		(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_app_upgrade_trigger:
 * @plugin: a #GsPlugin
 * @app: a #GsApp, with kind %AS_COMPONENT_KIND_OPERATING_SYSTEM
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Triggers the distribution upgrade to be installed on next boot.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_app_upgrade_trigger		(GsPlugin	*plugin,
							 GsApp		*app,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_file_to_app:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @file: a #GFile
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Converts a local file to a #GsApp. It's expected that only one plugin will
 * match the mimetype of @file and that a single #GsApp will be in the returned
 * list. If no plugins can handle the file, the list will be empty.
 *
 * For example, the PackageKit plugin can turn a .rpm file into a application
 * of kind %AS_COMPONENT_KIND_UNKNOWN but that in some cases it will be further refined
 * into a %AS_COMPONENT_KIND_DESKTOP_APP (with all the extra metadata) by the appstream
 * plugin.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_file_to_app			(GsPlugin	*plugin,
							 GsAppList	*list,
							 GFile		*file,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_url_to_app:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @url: a #URL, e.g. "apt://gimp"
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Converts a URL to a #GsApp. It's expected that only one plugin will
 * match the scheme of @url and that a single #GsApp will be in the returned
 * list. If no plugins can handle the file, the list will be empty.
 *
 * For example, the apt plugin can turn apt://gimp into a application.
 *
 * Plugins are expected to add new apps using gs_app_list_add().
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_url_to_app			(GsPlugin	*plugin,
							 GsAppList	*list,
							 const gchar	*url,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_update:
 * @plugin: a #GsPlugin
 * @apps: a #GsAppList
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Updates a list of applications, typically scheduling them for offline update.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_update			(GsPlugin	*plugin,
							 GsAppList	*apps,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_add_langpacks:
 * @plugin: a #GsPlugin
 * @list: a #GsAppList
 * @locale: a #LANGUAGE_CODE or #LOCALE, e.g. "ja" or "ja_JP"
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Returns a list of language packs, as per input language code or locale.
 *
 * Returns: %TRUE for success or if not relevant
 **/
gboolean	 gs_plugin_add_langpacks		(GsPlugin	*plugin,
							 GsAppList	*list,
							 const gchar    *locale,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_install_repo:
 * @plugin: a #GsPlugin
 * @repo: a #GsApp representing a repository
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Install the repository. This is a voluntary function, the plugin implements
 * it only if it supports it. If implemented, its pair function gs_plugin_remove_repo()
 * should be implemented as well.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @repo.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @repo
 * to either %GS_APP_STATE_INSTALLED or %GS_APP_STATE_AVAILABLE.
 *
 * Returns: %TRUE for success or if not relevant
 *
 * Since: 41
 **/
gboolean	 gs_plugin_install_repo			(GsPlugin	*plugin,
							 GsApp		*repo,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_remove_repo:
 * @plugin: a #GsPlugin
 * @repo: a #GsApp representing a repository
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Remove the repository. This is a voluntary function, the plugin implements
 * it only if it supports it. If implemented, its pair function gs_plugin_install_repo()
 * should be implemented as well.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @app.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @app
 * to %GS_APP_STATE_AVAILABLE or %GS_APP_STATE_UNKNOWN if not known.
 *
 * Returns: %TRUE for success or if not relevant
 *
 * Since: 41
 **/
gboolean	 gs_plugin_remove_repo			(GsPlugin	*plugin,
							 GsApp		*repo,
							 GCancellable	*cancellable,
							 GError		**error);
/**
 * gs_plugin_enable_repo:
 * @plugin: a #GsPlugin
 * @repo: a #GsApp representing a repository
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Enable the repository. This is a voluntary function, the plugin implements
 * it only if it supports it. If implemented, its pair function gs_plugin_disable_repo()
 * should be implemented as well.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @repo.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @repo
 * to %GS_APP_STATE_INSTALLED.
 *
 * Returns: %TRUE for success or if not relevant
 *
 * Since: 41
 **/
gboolean	 gs_plugin_enable_repo			(GsPlugin	*plugin,
							 GsApp		*repo,
							 GCancellable	*cancellable,
							 GError		**error);

/**
 * gs_plugin_disable_repo:
 * @plugin: a #GsPlugin
 * @repo: a #GsApp representing a repository
 * @cancellable: a #GCancellable, or %NULL
 * @error: a #GError, or %NULL
 *
 * Disable the repository. This is a voluntary function, the plugin implements
 * it only if it supports it. If implemented, its pair function gs_plugin_enable_repo()
 * should be implemented as well.
 *
 * Plugins are expected to send progress notifications to the UI using
 * gs_app_set_progress() using the passed in @app.
 *
 * All functions can block, but should sent progress notifications, e.g. using
 * gs_app_set_progress() if they will take more than tens of milliseconds
 * to complete.
 *
 * On failure the error message returned will usually only be shown on the
 * console, but they can also be retrieved using gs_plugin_loader_get_events().
 *
 * NOTE: Once the action is complete, the plugin must set the new state of @app
 * to %GS_APP_STATE_AVAILABLE.
 *
 * Returns: %TRUE for success or if not relevant
 *
 * Since: 41
 **/
gboolean	 gs_plugin_disable_repo			(GsPlugin	*plugin,
							 GsApp		*repo,
							 GCancellable	*cancellable,
							 GError		**error);
G_END_DECLS
