//--------------------------------------------------------------------------//
/// Copyright (c) 2019 by Milos Tosic. All Rights Reserved.                ///
/// License: http://www.opensource.org/licenses/BSD-2-Clause               ///
//--------------------------------------------------------------------------//

#include <rbase_pch.h>

#include <rbase/inc/itc.h>

namespace rtm {

CommandBuffer::CommandBuffer(uint32_t _bufferSize)
{
	m_commandBuffers[0].setCapacity(_bufferSize);
	m_commandBuffers[1].setCapacity(_bufferSize);
}

CommandBuffer::~CommandBuffer()
{
	RTM_ASSERT(!m_runThread, "");
}

void CommandBuffer::init(rtm::ThreadEntry _consumerFunc)
{
	m_consumerFunc = _consumerFunc;
	m_cmdBufferProduce = &m_commandBuffers[0];
	m_cmdBufferConsume = &m_commandBuffers[1];

	m_semConsume.post();

	m_cmdBufferProduce->start();

	frame();
	m_runThread = true;
	m_appThread.start(consumerThread, this);
}

void CommandBuffer::frame()
{
	m_cmdBufferProduce->finish();

	m_semConsume.wait();

	RWBuffer* tmp		= m_cmdBufferProduce;
	m_cmdBufferProduce	= m_cmdBufferConsume;
	m_cmdBufferConsume	= tmp;

	m_cmdBufferProduce->start();

	m_semProduce.post();
}

void CommandBuffer::shutDown()
{
	frame();
	m_runThread = false;
	m_appThread.stop();
}

int32_t CommandBuffer::consumerThread(void* _userData)
{
	CommandBuffer* cc = (CommandBuffer*)_userData;

	while (cc->m_runThread)
	{
		cc->m_semProduce.wait();
		cc->m_consumerFunc(cc);
		cc->m_semConsume.post();
	}
	return 0;
}

} // namespace rtm
