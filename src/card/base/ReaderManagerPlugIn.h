/*!
 * ReaderManagerPlugIn.h
 *
 * \brief PlugIn to control different kinds of reader managers that will be used in \ref ReaderManager.
 * If you implement a class of this PlugIn you need to register it in \ref ReaderManager, otherwise it won't be used.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "DeviceError.h"
#include "ReaderManagerPlugInInfo.h"

#include <QObject>
#include <QThread>

namespace governikus
{

class Reader;

class ReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT
	ReaderManagerPlugInInfo mInfo;

	protected:
		void setReaderInfoEnabled(bool pEnabled)
		{
			if (mInfo.isEnabled() != pEnabled)
			{
				mInfo.setEnabled(pEnabled);
				Q_EMIT fireStatusChanged(mInfo);
			}
		}


		void setReaderInfoAvailable(bool pAvailable)
		{
			mInfo.setAvailable(pAvailable);
		}


		void setReaderInfoValue(ReaderManagerPlugInInfo::Key pKey, const QVariant& pValue)
		{
			mInfo.setValue(pKey, pValue);
		}


	public:
		ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType,
				bool pAvailable = false,
				bool pPlugInEnabled = false);
		virtual ~ReaderManagerPlugIn() = default;

		const ReaderManagerPlugInInfo& getInfo() const
		{
			return mInfo;
		}


		virtual QList<Reader*> getReader() const = 0;


		virtual void init()
		{
			Q_ASSERT(thread() == QThread::currentThread());
		}


		virtual void shutdown()
		{
		}


		virtual void startScan()
		{
		}


		virtual void stopScan()
		{
		}


	Q_SIGNALS:
		void fireStatusChanged(const ReaderManagerPlugInInfo& pInfo);
		void fireReaderAdded(const QString& pReaderName);
		void fireReaderConnected(const QString& pReaderName);
		void fireReaderDeviceError(DeviceError pDeviceError);
		void fireReaderRemoved(const QString& pReaderName);
		void fireCardInserted(const QString& pReaderName);
		void fireCardRemoved(const QString& pReaderName);
		void fireCardRetryCounterChanged(const QString& pReaderName);
		void fireReaderPropertiesUpdated(const QString& pReaderName);
};

} /* namespace governikus */

Q_DECLARE_INTERFACE(governikus::ReaderManagerPlugIn, "governikus.ReaderManagerPlugIn")
