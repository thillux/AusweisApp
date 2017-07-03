/*
 * \brief Helper to get build date and time.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QtGlobal>

namespace governikus
{

class BuildHelper
{
	private:
		static const char* mDateTime;
		BuildHelper() = delete;
		~BuildHelper() = delete;

	public:
		static const char* getDateTime()
		{
			return mDateTime;
		}


#ifdef Q_OS_ANDROID
		static int getVersionCode();
		static int getVersionCode(const QString& pPackageName);

#endif


};

} /* namespace governikus */
