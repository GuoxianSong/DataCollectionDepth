#include"DataIO.h"
int main(int argc, char *argv[])
{

	DataIO dataio_;
	dataio_.Init();
	dataio_.Loop();
	dataio_.Stop();
	return 0;

}
