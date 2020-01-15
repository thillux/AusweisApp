/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "StateSelectReader.h"

#include "AppSettings.h"
#include "FuncUtils.h"
#include "ReaderManager.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateSelectReader::StateSelectReader(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


bool StateSelectReader::requiresCard(ReaderManagerPlugInType pPlugInType)
{
	return pPlugInType == ReaderManagerPlugInType::PCSC ||
		   pPlugInType == ReaderManagerPlugInType::REMOTE;
}


void StateSelectReader::run()
{
	const auto readerManager = Env::getSingleton<ReaderManager>();
	mConnections += connect(readerManager, &ReaderManager::fireReaderAdded, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(readerManager, &ReaderManager::fireReaderRemoved, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(readerManager, &ReaderManager::fireCardInserted, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(readerManager, &ReaderManager::fireCardRemoved, this, &StateSelectReader::onReaderInfoChanged);
	mConnections += connect(readerManager, &ReaderManager::fireReaderDeviceError, this, &StateSelectReader::onReaderDeviceError);

	onReaderInfoChanged();

	if (!Env::getSingleton<AppSettings>()->isUsedAsSDK())
	{
		const auto& readerPlugInTypes = Enum<ReaderManagerPlugInType>::getList();
		const auto& enabledPlugInTypes = getContext()->getReaderPlugInTypes();
		for (const auto t : readerPlugInTypes)
		{
			if (enabledPlugInTypes.contains(t))
			{
				readerManager->startScan(t);
				continue;
			}
			readerManager->stopScan(t);
		}
	}
}


void StateSelectReader::onReaderInfoChanged()
{
	const QSharedPointer<WorkflowContext> context = getContext();
	Q_ASSERT(context);
	bool currentReaderHasEidCardButInsufficientApduLength = false;

	const QVector<ReaderManagerPlugInType>& plugInTypes = context->getReaderPlugInTypes();
	const auto allReaders = Env::getSingleton<ReaderManager>()->getReaderInfos(ReaderFilter(plugInTypes));
	QVector<ReaderInfo> selectableReaders;

	for (const auto& info : allReaders)
	{
		if (info.isConnected() && (!requiresCard(info.getPlugInType()) || info.hasEidCard()))
		{
			if (info.sufficientApduLength())
			{
				selectableReaders.append(info);
			}
			else
			{
				currentReaderHasEidCardButInsufficientApduLength = true;
			}
		}
	}

	context->setCurrentReaderHasEidCardButInsufficientApduLength(currentReaderHasEidCardButInsufficientApduLength);

	if (selectableReaders.isEmpty())
	{
		qCDebug(statemachine) << "No selectable reader detected";

		return;
	}

	const ReaderInfo& readerInfo = selectableReaders.first();
	const QString readerName = readerInfo.getName();
	context->setReaderName(readerName);
	qCDebug(statemachine) << "Select first found reader:" << readerName;
	qCDebug(statemachine) << "Type:" << readerInfo.getPlugInType() << "BasicReader:" << readerInfo.isBasicReader();

	Q_EMIT fireContinue();
}


void StateSelectReader::onReaderDeviceError(const GlobalStatus& pErrorCode)
{
	if (pErrorCode.isError() && !pErrorCode.is(GlobalStatus::Code::Workflow_Reader_Device_Scan_Error))
	{
		updateStatus(pErrorCode);
		Q_EMIT fireAbort();
	}
}


void StateSelectReader::onEntry(QEvent* pEvent)
{
	const WorkflowContext* const context = getContext().data();
	Q_ASSERT(context);

	/*
	 * Note: the plugin types to be used in this state must be already set in the workflow context before this state is entered.
	 * Changing the plugin types in the context, e.g. from {NFC} to {BLUETOOTH}, causes the state to be left with a fireRetry signal.
	 */
	mConnections += connect(context, &WorkflowContext::fireReaderPlugInTypesChanged, this, &StateSelectReader::fireRetry);

	AbstractState::onEntry(pEvent);
}
