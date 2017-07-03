/*
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CommandLineParser.h"

#include "LanguageLoader.h"
#include "LogHandler.h"
#include "NetworkManager.h"
#include "SingletonHelper.h"
#include "view/UILoader.h"

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
#include "HttpServer.h"

#ifndef Q_OS_WINRT
	#include "GuiProfile.h"
#endif

#endif

#ifndef QT_NO_DEBUG
#include "UIPlugInWebSocket.h"
#endif

#include <QCoreApplication>
#include <QLoggingCategory>


using namespace governikus;

defineSingleton(CommandLineParser)

Q_DECLARE_LOGGING_CATEGORY(cmdline)

namespace
{
static QString prefixUi = QStringLiteral("UIPlugIn");

QString defaultUi(const QVector<UIPlugInName>& pPlugins)
{
	QStringList list;
	for (auto entry : pPlugins)
	{
		list << QString(getEnumName(entry)).remove(prefixUi);
	}
	return list.join(',');
}


}

CommandLineParser::CommandLineParser()
	: mParser()
	, mOptionKeepLog("keep", "Keep log file.")
	, mOptionShowWindow("show", "Show window on startup.")
	, mOptionProxy("no-proxy", "Disable system proxy.")
	, mOptionLanguage("language", "Force UI language.", LanguageLoader::getDefaultLanguage().uiLanguages().join(','))
	, mOptionUi("ui", "Use given UI plugin.", defaultUi(UILoader::getInstance().getDefault()))
	, mOptionPort("port", "Use listening port.", "24727")
#ifndef QT_NO_DEBUG
	, mOptionPortWebSocket("port-websocket", "Use listening port for websocket.", QString::number(UIPlugInWebSocket::WEBSOCKET_DEFAULT_PORT))
#endif
{
	addOptions();
}


CommandLineParser::~CommandLineParser()
{
}


CommandLineParser& CommandLineParser::getInstance()
{
	return *Instance;
}


void CommandLineParser::addOptions()
{
	mParser.addHelpOption();
	mParser.addVersionOption();

	mParser.addOption(mOptionKeepLog);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	mParser.addOption(mOptionShowWindow);
#endif

	mParser.addOption(mOptionProxy);
	mParser.addOption(mOptionLanguage);
	mParser.addOption(mOptionUi);
	mParser.addOption(mOptionPort);

#ifndef QT_NO_DEBUG
	mParser.addOption(mOptionPortWebSocket);
#endif
}


void CommandLineParser::parse(QCoreApplication* pApp)
{
	if (!pApp)
	{
		qCWarning(cmdline) << "QCoreApplication is undefined";
		return;
	}

	mParser.process(*pApp);
	parseUiPlugin();

	if (mParser.isSet(mOptionKeepLog))
	{
		LogHandler::getInstance().setAutoRemove(false);
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
	if (mParser.isSet(mOptionShowWindow))
	{
		GuiProfile::getProfile().setShowWindow(true);
	}
#endif

	if (mParser.isSet(mOptionProxy))
	{
		NetworkManager::lockProxy(true);
	}

	if (mParser.isSet(mOptionLanguage))
	{
		const auto& language = mParser.value(mOptionLanguage);
		const auto& locale = QLocale(language);
		if (locale == QLocale::C)
		{
			qCWarning(cmdline) << "Cannot detect language:" << language;
		}
		else
		{
			LanguageLoader::setDefaultLanguage(locale);
		}
	}

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	if (mParser.isSet(mOptionPort))
	{
		bool converted = false;
		const uint port = mParser.value(mOptionPort).toUInt(&converted);
		if (converted && port < USHRT_MAX)
		{
			HttpServer::cPort = static_cast<ushort>(port);
		}
		else
		{
			qCWarning(cmdline) << "Cannot use value as port:" << mParser.value(mOptionPort);
		}
	}
#endif

#ifndef QT_NO_DEBUG
	if (mParser.isSet(mOptionPortWebSocket))
	{
		bool converted = false;
		const uint port = mParser.value(mOptionPortWebSocket).toUInt(&converted);
		if (converted && port < USHRT_MAX)
		{
			UIPlugInWebSocket::setPort(static_cast<ushort>(port));
		}
		else
		{
			qCWarning(cmdline) << "Cannot use value as websocket port:" << mParser.value(mOptionPortWebSocket);
		}
	}
#endif
}


void CommandLineParser::parseUiPlugin()
{
	if (mParser.isSet(mOptionUi))
	{
		QVector<UIPlugInName> selectedPlugins;
		const auto& availablePlugins = Enum<UIPlugInName>::getList();
		const auto& requestedUis = mParser.values(mOptionUi);

		for (const auto& parsedUiOption : requestedUis)
		{
			for (auto availablePluginEntry : availablePlugins)
			{
				if (parsedUiOption.compare(QString(getEnumName(availablePluginEntry)).remove(prefixUi), Qt::CaseInsensitive) == 0)
				{
					selectedPlugins << availablePluginEntry;
				}
			}
		}

		if (!selectedPlugins.isEmpty())
		{
			UILoader::getInstance().setDefault(selectedPlugins);
		}
	}
}
