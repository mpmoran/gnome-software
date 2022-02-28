/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 * vi:set noexpandtab tabstop=8 shiftwidth=8:
 *
 * Copyright (C) 2018 Endless Mobile, Inc.
 *
 * Authors: Joaquim Rocha <jrocha@endlessm.com>
 *
 * SPDX-License-Identifier: GPL-2.0+
 */

#pragma once

#include "config.h"

#include <glib.h>
#include <gnome-software.h>

#define EXTERNAL_APPSTREAM_PREFIX "org.gnome.Software"

const gchar	*gs_external_appstream_utils_get_system_dir (void);
gchar		*gs_external_appstream_utils_get_file_cache_path (const gchar	*file_name);
void		 gs_external_appstream_refresh_async (guint64                     cache_age_secs,
						      GsDownloadProgressCallback  progress_callback,
						      gpointer                    progress_user_data,
						      GCancellable               *cancellable,
						      GAsyncReadyCallback         callback,
						      gpointer                    user_data);
gboolean	 gs_external_appstream_refresh_finish (GAsyncResult  *result,
						       GError       **error);
