
1����ʼ��ȱʡ��QP,ͳ�Ƴ�һ�����ʵ�QP
2��ͳ�Ƶ��������ܳ�����������,һ����������������Ϳ�ʼ����QP
3������СQP��ʱ��Ҫ�и��ȣ���С��С��ȷʡ��QP
4��������QP��ʱ��Ҫ�и��ȣ�������QPʱ�������ᳬ���������ʱ���ͽ�֡��
5�����ⲿ���ӿڣ������������ⲿ��̬���õ��������ܳ������õ�������
6������QP��ʱ��ӦҪ�죬��СQP��ʱ��Ҫ�º�


���Ը�����һP֡�Ĵ�С���Դ�Ź���һ�µ�ǰP֡�Ĵ�С
���Ը�����һI֡�Ĵ�С���Դ�Ź���һ�µ�ǰI֡�Ĵ�С

���� ��32 64 128 256 384 512 768 1024 1.5MB 2MB 4MB 6MB

��С4��ӹؼ��忪ʼͳ�ƣ�Ҫ��4���ڰ���һ���ؼ��壬�����һ���ؼ��塣
��С����4�룬���ܻẬ�ж�ؼ���

ÿ�ε�ͳ�� �ǹؼ���+�ӹؼ��忪ʼ�ĺ���4��ķǹؼ��� ����4 �ó��ġ�
��ֻ����������ͳ�ƣ�����С���趨ֵ��80%  �Ż��QP��1
��Щ����������Ҫȥͨ��QP���Զ�������ȥ�Զ������������Ҫ�����趨��ֵ��120%





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

