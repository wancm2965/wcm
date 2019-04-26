
1、初始化缺省的QP,统计出一个合适的QP
2、统计的码流不能超过最大的码流,一旦超出最大码流，就开始增加QP
3、当减小QP的时候要有个度，最小减小到确省的QP
4、当增大QP的时候要有个度，如果最大QP时码流还会超过最大码流时，就降帧率
5、给外部留接口，控制最大带宽，外部动态设置的最大带宽不能超过设置的最大带宽
6、增加QP的时候反应要快，减小QP的时候要温和


可以根据上一P帧的大小可以大概估计一下当前P帧的大小
可以根据上一I帧的大小可以大概估计一下当前I帧的大小

现在 是32 64 128 256 384 512 768 1024 1.5MB 2MB 4MB 6MB

最小4秒从关键桢开始统计，要在4秒内包容一个关键桢，最大到下一个关键桢。
最小必须4秒，可能会含有多关键桢

每次的统计 是关键桢+从关键桢开始的后面4秒的非关键桢 除以4 得出的。
而只有连续两次统计，码流小于设定值的80%  才会把QP减1
这些都是真正的要去通过QP的自动调整，去自动控制码流最大不要大于设定的值的120%





class IAVIFileRecorder
{
public:
	IAVIFileRecorder(void){};
	virtual~IAVIFileRecorder(void){};
public:
	virtual int Open(const char*cszPathName)=0;
	virtual void Close(void)=0;
	virtual void OnRecvdVideoPacket(unsigned char*pPacketData,int nPacketLen)=0;
	virtual void OnRecvdAudioSamples(unsigned char*pSamples,int nSamples,int nChannels,int nSampleRate,unsigned long nSynTimestamp)=0;
};

