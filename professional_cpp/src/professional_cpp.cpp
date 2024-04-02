// #include "professional_cpp.h"
//
// #include <debug.h>
//
// #include <KPluginFactory>
//
// K_PLUGIN_FACTORY_WITH_JSON(professional_cppFactory, "professional_cpp.json", registerPlugin<professional_cpp>(); )
//
// professional_cpp::professional_cpp(QObject *parent, const QVariantList& args)
//     : KDevelop::IPlugin(QStringLiteral("professional_cpp"), parent)
// {
//     Q_UNUSED(args);
//
//     qCDebug(PLUGIN_PROFESSIONAL_CPP) << "Hello world, my plugin is loaded!";
// }
//
// // needed for QObject class created from K_PLUGIN_FACTORY_WITH_JSON
// #include "professional_cpp.moc"
// #include "moc_professional_cpp.cpp"
