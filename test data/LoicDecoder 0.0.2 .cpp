#include <iostream>
#include <locale>
#include <string>
#include <fstream>
#include <math.h>

static const uint16_t crc16_tbl[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};
static const unsigned char i_dec[64] = {
	'#','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V',
	'W','X','Y','Z','#','#','#','#','#',' ','#','#','#','#','#','#','#','#','#','#','#','#','#',
	'#','#','0','1','2','3','4','5','6','7','8','9','#','#','#','#','#','#', };
static const std::string cap[8] = {
	"Транспондер уровня 1","Зарезервированный","Зарезервированный","Зарезервированный",
	"Транспондер уровня 2+, с возможностью установки CA на 7, наземный",
	"Транспондер уровня 2+ с возможностью установки CA на 7, бортовой",
	"Транспондер уровня 2+ с возможностью установки CA на 7, как на земле, так и в воздухе",
	"Означает, что значение запроса нисходящей линии связи равно 0 или статус полета равен 2, 3, 4 или 5, либо в воздухе, либо на земле.",
};
static const std::string tc2[7] = {
	"Surface emergency vehicle","_","Surface service vehicle","Ground obstruction","Ground obstruction","Ground obstruction","Ground obstruction",
};
static const std::string tc3[7] = {
	"Glider, sailplane","Lighter-than-air","Parachutist, skydiver","Ultralight, hang-glider, paraglider","Reserved","Unmanned aerial vehicle","Space or transatmospheric vehicle",
};
static const std::string tc4[7] = {
	"Light (less than 7000 kg)","Medium 1 (between 7000 kg and 34000 kg)","Medium 2 (between 34000 kg to 136000 kg)","High vortex aircraft","Heavy (larger than 136000 kg)","High performance (>5 g acceleration) and high speed (>400 kt)","Rotorcraft",
};
static const std::string ss_strs[4] = {
	"No condition","Permanent alert","Temporary alert","SPI condition",
};

uint16_t crc16(const uint8_t* buf, size_t sz)
{
    uint16_t crc;

    crc = 0xffff;

    while (sz--) {
        crc = (crc << 8) ^ crc16_tbl[(crc >> 8) ^ *(uint8_t*)buf++];
    }

    return crc;
}
void print_basic(FILE* s, uint8_t b5, uint8_t b6, uint8_t b7, uint8_t b8, uint8_t type, std::string str) {
	fprintf(s, "Формат связи(Downlink Format) = %u, \nВозможности(Capability) = %s(%u),\nИдентификатор судна ICAO = %02X%02X%02X, \nФормат данных = %s(%u)\n",
		(int)(b5 >> 3), cap[(int)(b5 >> 5)].c_str(), (int)(b5 >> 5), b6, b7, b8, str.c_str(), (int)type);
}
void print_1_4(FILE* s, uint8_t type, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_5_8(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_9_18(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_19(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_20_22(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_28(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_29(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);
void print_31(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15);

int main(){
	uint8_t block[18];
	uint8_t channel;
	uint32_t time;
	int8_t plev;
	uint16_t crc16_calculated;
	uint16_t crc16_stored;
    std::string finname = "", foutname = "";
    setlocale(LC_ALL, "RUS");
    std::cout << "Введите имя файла(без .loic):\n";
    while (finname == "")
    {
        std::cin >> finname;
    }
	std::cout << "Введите имя файла для вывода результата(без .txt):\n";
	while (foutname == "")
	{
		std::cin >> foutname;
	}
	std::FILE * fin, *fout;
    fopen_s(&fin, finname.append(".loic").data(), "rb");
	fopen_s(&fout, foutname.append(".txt").data(), "w");
    while (fread(block, sizeof(block), 1, fin) == 1) {
        channel = block[0];
        time = ((uint32_t)block[1] << 16) | ((uint32_t)block[2] << 8) | ((uint32_t)block[3] << 0);
		time = time / 128;
        plev = block[4];
        crc16_stored = ((uint16_t)block[16] << 8) | ((uint16_t)block[17] << 0);
        crc16_calculated = crc16(block, sizeof(block) - sizeof(uint16_t));
		fprintf(fout, "Channel=%u, Time=%u:%u:%u, Plev=%i, ", (unsigned int)channel, (unsigned int)time/3600, (time - ((unsigned int)time / 3600) * 3600)/60, time - ((unsigned int)time / 3600) * 3600 - ((time - ((unsigned int)time / 3600) * 3600) / 60)*60, (int)plev);
		fprintf(fout, "Msg=");
		short byte_num = 0;
		for (int i = 5; i < 16; ++i) {
			fprintf(fout, "%02X", block[i]);
		}
		fprintf(fout, ", CRC16=0x%04X (%s)\n", crc16_stored, (crc16_stored == crc16_calculated) ? "Ok" : "Wrong!");
		// Тип сообщения
		uint8_t type = (int)(block[9] >> 3);
		std::string s_type;
		switch (type)
		{
		case 1:case 2:case 3:case 4:
			s_type = "Идентификация самолета(Aircraft identification)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_1_4(fout, type, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 5:case 6:case 7:case 8:
			s_type = "Положение на поверхности(Surface position)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_5_8(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 9:case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:
			s_type = "Положение в воздухе (с барометрической высотой)(Airborne position (w/Baro Altitude))";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_9_18(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 19:
			s_type = "Скорость в воздухе(Airborne velocities)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_19(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 20:case 21:case 22:
			s_type = "Положение в воздухе (с высотой GNSS)(Airborne position (w/GNSS Height))";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_20_22(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 23:case 24:case 25:case 26:case 27:
			s_type = "Зарезервировано(Reserved)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			break;
		case 28:
			s_type = "Состояние самолета(Aircraft status)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_28(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 29:
			s_type = "Состояние и статус цели(Target state and status information)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_29(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		case 31:
			s_type = "Рабочий статус самолета(Aircraft operation status)";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			print_31(fout, block[9], block[10], block[11], block[12], block[13], block[14], block[15]);
			break;
		default:
			s_type = "Error";
			print_basic(fout, block[5], block[6], block[7], block[8], type, s_type);
			break;
		}
		fprintf(fout, "\n");
		/*uint16_t tmp = (uint16_t)block[10];
		for (int i = 0; i < 16; i++, tmp <<= 1) {
			fprintf(fout, "%c", (char)(48 + (tmp >> 15)));
		}
		fprintf(fout, "\n");
		tmp = block[10] << 8;
		tmp += block[11];
		tmp >>= 4;
		for (int i = 0; i < 16; i++, tmp <<= 1) {
			fprintf(fout, "%c", (char)(48 +(tmp>>15)));
		}
		fprintf(fout, "\n");*/
	}
}

void print_1_4(FILE* s, uint8_t type, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	uint8_t ca = b9 << 5;
	ca >>= 5;

	uint8_t l1 = b10 >> 2;
	uint8_t l2 = b10 << 6;
	l2 += b11 >> 2;
	l2 >>= 2;
	uint8_t l3 = b11 << 4;
	l3 += b12 >> 4;
	l3 >>= 2;
	uint8_t l4 = b12 << 2;
	l4 >>= 2;

	uint8_t l5 = b13 >> 2;
	uint8_t l6 = b13 << 6;
	l6 += b14 >> 2;
	l6 >>= 2;
	uint8_t l7 = b14 << 4;
	l7 += b15 >> 4;
	l7 >>= 2;
	uint8_t l8 = b15 << 2;
	l8 >>= 2;
	fprintf(s, "Категория объекта: ");
	if (type == 1) fprintf(s, "Reserved(CA %u)", (int)ca);
	if (type == 2) fprintf(s, "%s(CA %u)", tc2[ca].c_str(), (int)ca);
	if (type == 3) fprintf(s, "%s(CA %u)", tc3[ca].c_str(), (int)ca);
	if (type == 4) fprintf(s, "%s(CA %u)", tc4[ca].c_str(), (int)ca);
	fprintf(s, "\nИдентификатор: %c%c%c%c%c%c%c%c\n", i_dec[l1],
		i_dec[l2], i_dec[l3], i_dec[l4], i_dec[l5],
		i_dec[l6], i_dec[l7], i_dec[l8]);
}

void print_5_8(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	uint8_t mov = b9 << 5;
	mov += b10 >> 3;
	mov >>= 1;
	if (mov)
	{
		if (mov == 1)
		{
			fprintf(s, "Movement = Stopped\n");
		}
		else
		{
			double speed = 0.0;
			if (2 <= mov < 9)
			{
				speed = 0.125 + ((uint16_t)mov + 1 - 2) * 0.125;
			}
			if (9 <= mov < 13)
			{
				speed = 1 + ((uint16_t)mov + 1 - 9) * 0.25;
			}
			if (13 <= mov < 39)
			{
				speed = 2 + ((uint16_t)mov + 1 - 13) * 0.5;
			}
			if (39 <= mov < 94)
			{
				speed = 15 + ((uint16_t)mov + 1 - 39) * 1;
			}
			if (94 <= mov < 109)
			{
				speed = 70 + ((uint16_t)mov + 1 - 94) * 2;
			}
			if (109 <= mov < 124)
			{
				speed = 100 + ((uint16_t)mov + 1 - 109) * 5;
			}
			fprintf(s, "Movement = %f kt\n", speed);
		}
	}
	else
	{
		fprintf(s, "Movement = NaN\n");
	}

	uint8_t g_t = b10 << 4;
	g_t += b11 >> 4;
	if (g_t >> 7) {
		g_t <<= 1;
		g_t >>= 1;
		fprintf(s, "Ground track = %f\n", ((float)(360*g_t))/128);
	}
	else {
		fprintf(s, "Ground track = INVALID\n");
	}

	uint8_t time = b11 >> 3;
	time <<= 7;
	time >>= 7;
	fprintf(s, "Time = %u\n", (int)time);
	uint8_t cpr = b11 >> 2;
	cpr <<= 7;
	cpr >>= 7;
	fprintf(s, "CPR = %u\n", (int)cpr);
	uint32_t lat_cpr = b11 << 6;
	lat_cpr <<= 2;
	lat_cpr += b12;
	lat_cpr <<= 7;
	lat_cpr += b13 >> 1;
	fprintf(s, "RAW Latitude = %u\n", (int)lat_cpr);
	uint32_t lon_cpr = b13 << 7;
	lon_cpr <<= 1;
	lon_cpr += b14;
	lon_cpr <<= 8;
	lon_cpr += b15;
	fprintf(s, "RAW Longitude = %u\n", (int)lon_cpr);
}

void print_9_18(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	uint8_t ss = b9 << 5;
	ss >>= 6;
	fprintf(s, "Surveillance status = %s(%u)\n", ss_strs[ss].c_str() , (int)ss);
	uint8_t saf = b9 << 7;
	saf >>= 7;
	fprintf(s, "Single antenna flag = %u\n", (int)saf);
	uint8_t mm = b10 >> 1;
	uint16_t alt = mm;
	alt <<= 4;
	mm = b11 >> 4;
	alt += mm;
	if (alt) {
		if (b10 >> 7) // 25
		{
			alt = alt * 25 - 1000;
		}
		else // 100
		{
			uint16_t gr = 0, tmp = alt;
			for (; tmp; tmp >>= 1) gr ^= tmp; 
			alt = gr;
			alt = alt * 100;
		}
		alt *= 0.3048;
		fprintf(s, "Altitude = %u meters\n", (int)alt);
	}
	else {
		fprintf(s, "Altitude = NaN\n");
	}
	uint8_t time = b11 >> 3;
	time <<= 7;
	time >>= 7;
	fprintf(s, "Time = %u\n", (int)time);
	uint8_t cpr = b11 >> 2;
	cpr <<= 7;
	cpr >>= 7;
	fprintf(s, "CPR = %u\n", (int)cpr);
	uint32_t lat_cpr = b11 << 6;
	lat_cpr <<= 2;
	lat_cpr += b12;
	lat_cpr <<= 7;
	lat_cpr += b13 >> 1;
	fprintf(s, "RAW Latitude = %u\n", (int)lat_cpr);
	uint32_t lon_cpr = b13 << 7;
	lon_cpr <<= 1;
	lon_cpr += b14;
	lon_cpr <<= 8;
	lon_cpr += b15;
	fprintf(s, "RAW Longitude = %u\n", (int)lon_cpr);
}

void print_19(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	float angle = 0.0;
	int32_t horizontal_speed = 0, vertical_speed = 0, gnss_bar_diff = 0;
	uint8_t sub_type = b9 << 5;
	sub_type >>= 5;
	uint8_t int_ch_f = b10 >> 7; // Intent change flag
	uint8_t ifr_cap_f = b10 << 1; // IFR capability flag
	ifr_cap_f >>= 7;
	uint8_t nuc = b10 << 2; // Navigation uncertainty category for velocity
	nuc >>= 5;
	/*
		ADS-B version 0		NUCr			
		ADS-B version 1–2	NUCv	
	*/
	if (sub_type == 1 || sub_type == 2)
	{
		uint8_t dir_ew_c = b10 << 5; // Direction for E-W velocity component
		dir_ew_c >>= 7;
		/*
			0: from West to East
			1 : from East to West
		*/
		uint8_t tmp = b10 << 6;
		uint16_t dir_ew_v = tmp; // East-West velocity component
		dir_ew_v <<= 2;
		dir_ew_v += b11;
		/*
			All zeros: no information available				
			Sub-type 1: Speed = Decimal value - 1				
			Sub-type 2: Speed = 4 x (Decimal value - 1)				
			Unit: Knots
		*/
		int16_t speed_ew = dir_ew_v - 1;;
		if (dir_ew_c)
		{
			speed_ew = - speed_ew;
		}
		uint8_t dir_ns_c = b12 >> 7; // Direction for N-S velocity component
		/*
			0: from South to North				
			1: from North to South
		*/
		tmp = b12 << 1;
		uint16_t dir_ns_v = tmp;
		dir_ns_v <<= 2;
		dir_ns_v += b13 >> 5;
		/*
			All zeros: No information available				
			Sub-type 1: Speed = Decimal value - 1				
			Sub-type 2: Speed = 4 x (Decimal value - 1)				
			Unit: Knots
		*/
		int16_t speed_ns = dir_ns_v - 1;
		if (dir_ns_c)
		{
			speed_ns = -speed_ns;
		}
		if (sub_type == 2)
		{
			speed_ns *= 4;
			speed_ew *= 4;
		}
		// Speed and angle calculation
		horizontal_speed = sqrt((pow(speed_ew, 2) + pow(speed_ns, 2)));
		angle = atan2(speed_ew, speed_ns) * 360 / (2 * 3.14159265359);
	}
	if (sub_type == 3 || sub_type == 4)
	{
		uint8_t s_m_head = b10 << 5; // Status bit for magnetic heading
		s_m_head >>= 7;
		/*
			0: not available
			1: available
		*/
		uint8_t tmp = b10 << 6;
		uint16_t m_head = tmp; // Magnetic heading
		m_head <<= 2;
		m_head += b11;
		/*
			LSB: 360/1024 degrees				
			Heading = Decimal value x 360 / 1024
		*/
		uint8_t airs_type = b12 >> 7; // Airspeed type
		/*
			0: Indicated airspeed (IAS)			
			1: True airspeed (TAS)
		*/
		tmp = b12 << 1;
		uint16_t airs = tmp;
		airs <<= 2;
		airs += b13 >> 5;
		/*
			All zeros: no information available				
			Sub-type 3: Speed = Decimal value - 1				
			Sub-type 4: Speed = 4 x (Decimal value - 1)				
			Unit: knots
		*/
		// Speed and angle calculation
		horizontal_speed = airs - 1;
		if (sub_type == 4)
		{
			horizontal_speed *= 4;
		}
		angle = m_head * 360 / 1024;
	}
	uint8_t vr_type = b13 << 3; // Source bit for vertical rate
	vr_type >>= 7;
	/*
		0: GNSS 
		1: Barometer
	*/
	uint8_t vr_dir = b13 << 4; // Sign bit for vertical rate
	vr_type >>= 7;
	/*
		0: Up 
		1: Down
	*/
	uint8_t tmp = b13 << 5;
	int16_t vr = tmp; // Vertical rate
	vr <<= 1;
	tmp = b14 >> 2;
	vr += tmp;
	if (vr_dir)
	{
		vertical_speed = -(64 * (vr - 1));
	}
	else
	{
		vertical_speed = 64 * (vr - 1);
	}
	uint8_t sgn_alt = b15 >> 7; // Sign bit for GNSS and Baro altitudes difference
	/*
		0: GNSS alt above Baro alt				
		1: GNSS alt below Baro alt
	*/
	uint8_t dif_alt = b15 << 1; // Difference between GNSS and Baro altitudes
	dif_alt >>= 1;
	/*
		All zeros: no information				
		LSB: 25 ft
	*/
	if (sgn_alt)
	{
		gnss_bar_diff = -((dif_alt - 1) * 25);
	}
	else
	{
		gnss_bar_diff = (dif_alt - 1) * 25;
	}
	fprintf(s, "Vertical rate = %d ft/min \n", vertical_speed);
	fprintf(s, "Horizontal speed = %d\n", horizontal_speed);
	fprintf(s, "Angle = %f\n", angle);
	if (vr_type)
	{
		fprintf(s, "Vertical speed type = Barometer\n");
	}
	else
	{
		fprintf(s, "Vertical speed type = GNSS\n");
	}
	fprintf(s, "GNSS and barometric altitudes difference = %d\n", gnss_bar_diff);
}

void print_20_22(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	uint8_t ss = b9 << 5;
	ss >>= 6;
	fprintf(s, "Surveillance status = %s(%u)\n", ss_strs[ss].c_str(), (int)ss);
	uint8_t saf = b9 << 7;
	saf >>= 7;
	fprintf(s, "Single antenna flag = %u\n", (int)saf);
	uint16_t alt = b10;
	alt <<= 4;
	alt += b11 >> 4;
	fprintf(s, "Altitude = %u meters\n", (int)alt);
	uint8_t time = b11 >> 3;
	time <<= 7;
	time >>= 7;
	fprintf(s, "Time = %u\n", (int)time);
	uint8_t cpr = b11 >> 2;
	cpr <<= 7;
	cpr >>= 7;
	fprintf(s, "CPR = %u\n", (int)cpr);
	uint32_t lat_cpr = b11 << 6;
	lat_cpr <<= 2;
	lat_cpr += b12;
	lat_cpr <<= 7;
	lat_cpr += b13 >> 1;
	fprintf(s, "RAW Latitude = %u\n", (int)lat_cpr);
	uint32_t lon_cpr = b13 << 7;
	lon_cpr <<= 1;
	lon_cpr += b14;
	lon_cpr <<= 8;
	lon_cpr += b15;
	fprintf(s, "RAW Longitude = %u\n", (int)lon_cpr);
}

void print_28(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	fprintf(s, "Work in progres\n");
}

void print_29(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	fprintf(s, "Work in progres\n");
}

void print_31(FILE* s, uint8_t b9, uint8_t b10, uint8_t b11, uint8_t b12, uint8_t b13, uint8_t b14, uint8_t b15)
{
	uint8_t sub_type = b9 << 5; // Sub-type cod
	sub_type >>= 5;
	/*
		0: Airborne status message
		1: Surface status message
	*/
	if (sub_type == 0)
	{
		fprintf(s, "Airborne status message\n");
	}
	if (sub_type ==	1)
	{
		fprintf(s, "Surface status message\n");
	}
	uint8_t adsb_ver = b14 >> 5; // ADS-B version number
	/*
		0: Comply with DOC 9871, Appendix A				
		1: Comply with DOC 9871, Appendix B				
		2: Comply with DOC 9871, Appendix C				
		3–7: Reserved
	*/
	fprintf(s, "ADS-B version = %d\n", (int)adsb_ver);
	if (adsb_ver != 0)
	{
		uint16_t cap_codes = b10; // Capacity class codes
		cap_codes <<= 8;
		cap_codes += b11;
		/*
			ST=0: Airborne capacity class codes (16 bits)				
			ST=1: Surface capacity codes (12 bits)				
			+ Length/width code (4 bits)
		*/
		uint16_t oper_codes = b12; // Operational mode codes
		oper_codes <<= 8;
		oper_codes += b13;
		/*
			ST=0: Airborne operational mode codes				
			ST=1: Surface operational mode codes
		*/
		if (sub_type == 0)
		{
			fprintf(s, "Airborne capacity class codes = ");
			for (int i = 0; i < 16; i++, cap_codes <<= 1) {
				fprintf(s, "%c", (char)(48 + (cap_codes >> 15)));
			}
			fprintf(s, "\n");

		}
		if (sub_type == 1)
		{
			fprintf(s, "Surface capacity codes = ");
			for (int i = 0; i < 12; i++, cap_codes <<= 1) {
				fprintf(s, "%c", (char)(48 + (cap_codes >> 11)));
			}
			fprintf(s, "\n");
			fprintf(s, "Length/width code = ");
			for (int i = 0; i < 4; i++, cap_codes <<= 1) {
				fprintf(s, "%c", (char)(48 + (cap_codes >> 3)));
			}
			fprintf(s, "\n");
		}
		if (adsb_ver == 1) // 1
		{
			fprintf(s, "Operational mode codes = ");
			for (int i = 0; i < 16; i++, oper_codes <<= 1) {
				fprintf(s, "%c", (char)(48 + (oper_codes >> 15)));
			}
			fprintf(s, "\n");
			uint8_t nic_sup = b14 << 3; // NIC supplement
			nic_sup >>= 7;
			fprintf(s, "NIC supplement = %d\n", (int)nic_sup);
			uint8_t nav_acc_cat = b14 << 4; // Navigational accuracy category - position
			nav_acc_cat >>= 4;
			fprintf(s, "Navigational accuracy category - position = ");
			for (int i = 0; i < 4; i++, cap_codes <<= 1) {
				fprintf(s, "%c", (char)(48 + (cap_codes >> 3)));
			}
			fprintf(s, "\n");
			if (sub_type == 0)
			{
				uint8_t bar_alt_q = b15 >> 6;
				fprintf(s, "Barometric altitude quality = %d\n", (int)bar_alt_q);
			}
			uint8_t surv_int_level = b15 << 2;
			surv_int_level >>= 6;
			fprintf(s, "Surveillance integrity level = %d\n", (int)surv_int_level);
			uint8_t bar_alt_int = b15 << 3;
			bar_alt_int >>= 7;
			if (sub_type == 0)
			{
				fprintf(s, "Barometric altitude quality = %d\n", (int)bar_alt_int);
			}
			if (sub_type == 1)
			{
				fprintf(s, "Track angle or heading = %d\n", (int)bar_alt_int);
			}
			uint8_t hor_ref_dir = b15 << 4;
			hor_ref_dir >>= 7;
			fprintf(s, "Horizontal reference direction = %d\n", (int)hor_ref_dir);
		}
		else // 2
		{
			if (sub_type == 0)
			{
				fprintf(s, "Airborne operational mode codes = ");
				for (int i = 0; i < 16; i++, oper_codes <<= 1) {
					fprintf(s, "%c", (char)(48 + (oper_codes >> 15)));
				}
				fprintf(s, "\n");
			}
			if (sub_type == 1)
			{
				fprintf(s, "Surface operational mode codes = ");
				for (int i = 0; i < 16; i++, oper_codes <<= 1) {
					fprintf(s, "%c", (char)(48 + (oper_codes >> 15)));
				}
				fprintf(s, "\n");
			}
			uint8_t nic_sup = b14 << 3; // NIC supplement - A
			nic_sup >>= 7;
			fprintf(s, "NIC supplement - A = %d\n", (int)nic_sup);
			uint8_t nav_acc_cat = b14 << 4; // Navigational accuracy category - position
			nav_acc_cat >>= 4;
			fprintf(s, "Navigational accuracy category - position = ");
			for (int i = 0; i < 4; i++, cap_codes <<= 1) {
				fprintf(s, "%c", (char)(48 + (cap_codes >> 3)));
			}
			fprintf(s, "\n");
			if (sub_type == 0)
			{
				uint8_t geom_vert_acc = b15 >> 6;
				fprintf(s, "Geometric vertical accuracy = %d\n", (int)geom_vert_acc);
			}
			uint8_t sour_int_level = b15 << 2;
			sour_int_level >>= 6;
			fprintf(s, "Source integrity level = %d\n", (int)sour_int_level);
			uint8_t bar_alt_int = b15 << 3;
			bar_alt_int >>= 7;
			if (sub_type == 0)
			{
				fprintf(s, "Barometric altitude integrity = %d\n", (int)bar_alt_int);
			}
			if (sub_type == 1)
			{
				fprintf(s, "Track angle or heading = %d\n", (int)bar_alt_int);
			}
			uint8_t hor_ref_dir = b15 << 4;
			hor_ref_dir >>= 7;
			fprintf(s, "Horizontal reference direction = %d\n", (int)hor_ref_dir);
			uint8_t sil_sup = b15 << 5;
			sil_sup >>= 7;
			fprintf(s, "SIL supplement = %d\n", (int)sil_sup);
		}
	}
}
