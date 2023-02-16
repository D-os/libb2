#include <Window.h>

class BPrivateScreen
{
   public:
	virtual ~BPrivateScreen(){};

	virtual BRect Frame() = 0;
};
