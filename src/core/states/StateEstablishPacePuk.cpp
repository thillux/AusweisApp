/*!
 * StateEstablishPacePuk.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */


#include "StateEstablishPacePuk.h"

#include "context/ChangePinContext.h"


using namespace governikus;

StateEstablishPacePuk::StateEstablishPacePuk(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateEstablishPacePuk::run()
{
	auto cardConnection = getContext()->getCardConnection();

	Q_ASSERT(cardConnection);
	qDebug() << "Invoke unblock PIN command";
	mConnections += cardConnection->callUnblockPinCommand(this, &StateEstablishPacePuk::onEstablishConnectionDone, getContext()->getPuk());
	getContext()->setPuk(QString());
}


void StateEstablishPacePuk::onUserCancelled()
{
	getContext()->setLastPaceResultAndRetryCounter(CardReturnCode::CANCELLATION_BY_USER, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
	AbstractState::onUserCancelled();
}


void StateEstablishPacePuk::onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand)
{
	CardReturnCode returnCode = pCommand->getReturnCode();
	switch (returnCode)
	{
		case CardReturnCode::OK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			if (auto changePinContext = getContext().objectCast<ChangePinContext>())
			{
				changePinContext->setSuccessMessage(tr("PIN successfully unblocked"));
			}
			Q_EMIT fireSuccess();
			break;

		case CardReturnCode::CANCELLATION_BY_USER:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireCancel();
			break;

		case CardReturnCode::INVALID_PUK:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			Q_EMIT fireInvalidPuk();
			break;

		case CardReturnCode::PUK_INOPERATIVE:
			getContext()->setLastPaceResultAndRetryCounter(returnCode, getContext()->getCardConnection()->getReaderInfo().getRetryCounter());
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireInoperativePuk();
			break;

		default:
			setStatus(CardReturnCodeUtil::toGlobalStatus(returnCode));
			Q_EMIT fireError();
			break;
	}
}
