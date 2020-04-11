#ifndef GRAPHICSRULESMAKER_CONFIG_H
#define GRAPHICSRULESMAKER_CONFIG_H

#if defined(GRAPHICSRULESMAKER_LIBRARY)
#  define GRAPHICSRULESMAKER_EXPORT Q_DECL_EXPORT
#else
#  define GRAPHICSRULESMAKER_EXPORT Q_DECL_IMPORT
#endif

/**
 * The installation directory for the default plugins
 */
#define GRAPHICSRULESMAKER_PLUGIN_PATH "@GraphicsRulesMaker_PLUGIN_PATH@"

/**
 * The installation directory for translation files
 */
#define GRAPHICSRULESMAKER_TRANSLATIONS_PATH "@GraphicsRulesMaker_TRANSLATIONS_PATH@"

/**
 * The library version
 */
#define GRAPHICSRULESMAKER_VERSION "@GraphicsRulesMaker_VERSION@"

/**
 * The last copyright year
 */
#define GRAPHICSRULESMAKER_COPYRIGHT_YEAR "@GraphicsRulesMaker_COPYRIGHT_YEAR@"

#endif // GRAPHICSRULESMAKER_CONFIG_H

