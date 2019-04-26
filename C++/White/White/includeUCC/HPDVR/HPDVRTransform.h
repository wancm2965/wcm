#pragma once

class IHPDVRTransformCallBack
{
public:
	virtual void OnVideoDelivered(char*data, int len, bool key, unsigned long timestamp) = 0;
	virtual void OnAudioDelivered(char*data, int len, bool key, unsigned long timestamp) = 0;
	virtual void OnVideoSizeChanged(int width, int height) = 0;
};

class IHPDVRTransform
{ 
public:
	virtual bool Connect(long handle) = 0;
	virtual void Release() = 0;
	virtual bool IsConnected() = 0;
	virtual void InputStreamDelivered(char*data, int len, int flag, long head) = 0;
	virtual void InputVoiceDelivered(char*data, int len) = 0;
};
