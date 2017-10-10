/* (c) 2007 by Michal Cihar */

/** \file locales.h
 * Gettext wrapper.
 *
 * @author Michal Cihar
 * @{
 */

#ifndef __LOCALES_H
#define __LOCALES_H

#include <gammu-config.h>

#include <locale.h>

#ifdef LIBINTL_LIB_FOUND
#include <libintl.h>
#define _(x) dgettext("libgammu", x)
#else
#define _(x) (x)
#define dgettext(d, x) (x)
#define ngettext(singular, plural, number) (number == 1 ? singular : plural)
#endif

#define N_(x) x

#endif
