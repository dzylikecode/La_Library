#include "La_Blink.h"
#include "La_Palette.h"

namespace GRAPHIC
{
	int BLINKER::command(LIGHT& newLight, OPERATOR opera, int id)
	{
		switch (opera)
		{
		case GRAPHIC::BLINKER::Add:
			for (int i = 0; i < blinkerNum; i++)
			{
				if (light[i].state == LIGHT::Available)
				{
					light[i] = newLight;
					light[i].state = LIGHT::Off;
					light[i].counter = 0;
					curPalette[light[i].color_index] = light[i].offCol;

					return i;
				}
			}
			//说明不可能添加了
			return -1;
			break;
		case GRAPHIC::BLINKER::Delete:
			if (light[id].state != LIGHT::Available)
			{
				light[id].state = LIGHT::Available;
				return id;
			}
			else
			{
				return -1;
			}

			break;
		case GRAPHIC::BLINKER::Update:
			if (light[id].state != LIGHT::Available)
			{
				LIGHT::STATE temp = light[id].state;
				light[id] = newLight;
				light[id].state = temp;

				if (light[id].state == LIGHT::Off)
				{
					curPalette[light[id].color_index] = light[id].offCol;
				}
				else
				{
					curPalette[light[id].color_index] = light[id].onCol;
				}
				return id;
			}
			else
			{
				return -1;
			}

			break;
		case GRAPHIC::BLINKER::Run:
			for (int i = 0; i < blinkerNum; i++)
			{
				if (light[i].state == LIGHT::Off)
				{
					if (++light[i].counter >= light[i].off_time)
					{
						light[i].counter = 0;

						light[i].state = LIGHT::On;
						curPalette[light[id].color_index] = light[id].onCol;
					}
				}
				else if (light[i].state == LIGHT::On)
				{
					if (++light[i].counter >= light[i].on_time)
					{
						light[i].counter = 0;

						light[i].state = LIGHT::Off;
						curPalette[light[id].color_index] = light[id].offCol;
					}
				}
			}
			break;
		default:
			break;
		}

		return 0;
	}

	
}