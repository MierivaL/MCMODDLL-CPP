#include "pch.h"
#include "mod.h"
#include "SymHook.h"
#include "Minecraft.hpp"
using namespace SymHook;

/**����������������������������������������������������������������������������**
 |              MOD C++�ļ�               |
 **����������������������������������������������������������������������������**
˵����
��cpp�ļ���Ŀ���ǰ���MOD������Ҫ���룬�뽫��Ҫ���ִ���д������ļ��ڣ�
��T����ͷ��ϵ��Hook����ģ����ص�ʱ����ã�ʹ�÷�����ο�MCMrARM��modloader��
��ַ��https://github.com/minecraft-linux/server-modloader/wiki
��ע�⣺ʹ�÷������в�ͬ�����ļ��ĺ��������õ���PDB�������ɵ�C++�������������ַ�����������
���⣬���ļ�������������mod_init��mod_exit���ֱ��ڸ�ģ����ػ��˳�ʱ���á�
��ģ�����ӣ�
THook(void,							// ������������
	MSSYM_XXXXXXXXXXXXXXXXXXXXX,	// ������������Ӧ��C++������λ��SymHook.h��
	__int64 a1, __int64 a2) {		// �������������б���������ڲ���a1��a2��
	std::cout << "Hello world!" << std::endl;
	original(a1, a2);				// ���øú���Hookǰ��ԭʼ����
}
*/
// �˴���ʼ��дMOD����

#include <map>
#include <unordered_map>
#include <string>
#include <chrono>

std::map<short, std::string> BlockRegMap;

static std::string UTF8ToGBK(const char* strUTF8);

// ע�᷽���ʱ�򹹽�����ID���ұ�
THook(void,
	MSSYM_B1QE14registerBlocksB1AE17VanillaBlockTypesB2AAA5YAXXZ,
	void* _this) {
	original(_this);
	std::unordered_map<std::string, SharedPtr<BlockLegacy>>* pMap;
	pMap = SYM_PTR(decltype(pMap), MSSYM_MD5_ceb8b47184006e4d7622b39978535236);
	for (auto i = pMap->begin(); i != pMap->end(); ++i) {
		auto id = i->second->getBlockItemID();
		auto str = std::string("Mincraft: " + id);
			//i->second->getFullName();
		BlockRegMap.emplace(id, str);
	}
}

namespace Log {
	namespace Helper {
		template<size_t size>
		void UtoA_Fill(char (&buf)[size], int num) {
			int nt = size - 1;
			buf[nt] = 0;
			for (auto i = nt - 1; i >= 0; --i) {
				char d = '0' + (num % 10);
				num /= 10;
				buf[i] = d;
			}
		}

		auto TimeNow() {
			auto timet = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			tm time;
			char buf[3] = { 0 };
			localtime_s(&time, &timet);
			std::string str(std::to_string((time.tm_year + 1900)));
			str += "-";
			UtoA_Fill(buf, time.tm_mon + 1);
			str += buf; str += "-";
			UtoA_Fill(buf, time.tm_mday);
			str += buf; str += " ";
			UtoA_Fill(buf, time.tm_hour);
			str += buf; str += ":";
			UtoA_Fill(buf, time.tm_min);
			str += buf; str += ":";
			UtoA_Fill(buf, time.tm_sec);
			str += buf;
			return str;
		}

		auto Title(const std::string& content) {
			return std::string("{[") + TimeNow() + " " + content + "]";
		}
		auto Coordinator(INT32 coordinator[]) {
			return std::string("(")
				+ std::to_string(coordinator[0]) + ", "
				+ std::to_string(coordinator[1]) + ", "
				+ std::to_string(coordinator[2]) + ")";
		}

		auto Dimension(int v) {
			switch (v) {
			case 0: return "������";
			case 1: return "����";
			case 2: return "ĩ��";
			}
			return "δ֪ά��";
		}
	}

	namespace Player {
		using namespace Helper;

		void Error(const std::string& title, const std::string& player_name, int dimension, const std::string& content) {
			std::cout
				<< Title(title) << " "
				<< "���" << " " << player_name << " "
				<< "��" << Dimension(dimension)
				<< content << std::endl;
		}
		void Block(const std::string& title, const std::string& player_name, int dimension, const std::string& operation, const short block_id, INT32 coordinator[]) {
			auto block_name_inner = block_id;
//			if (block_id == 0)
//				block_name_inner = -1;//"δ֪����";
			std::cout
				<< Title(title) << " "
				<< "���" << " " << player_name << " "
				<< "��" <<Dimension(dimension)<< " " << Coordinator(coordinator) << " "
				<< operation << " id:"
				<< block_name_inner << " " << "���顣"
				<< std::endl;
		}
		void Interaction(const std::string& title, const std::string& player_name, int dimension, const std::string& operation, const std::string& object_name, INT32 coordinator[]) {
			std::cout
				<< Title(title) << " "
				<< "���" << " " << player_name << " "
				<< operation
				<< "��" << Dimension(dimension) << " " << Coordinator(coordinator) << " " << "��"
				<< object_name << " " << "��"
				<< std::endl;
		}
		void Container_In(const std::string& title, const std::string& player_name, int dimension, int slot, int count, const std::string& object_name) {
			std::cout
				<< Title(title) << " "
				<< "���" << " " << player_name << " "
				<< "��" << " " << slot << " " << "����"
				<< "����" << " "
				<< count << " " << "��" << " "
				<< UTF8ToGBK(object_name.data()) << " " << "��Ʒ��"
				<< std::endl;
		}
		void Container_Out(const std::string& title, const std::string& player_name, int dimension, int slot) {
			std::cout
				<< Title(title) << " "
				<< "���" << " " << player_name << " "
				<< "ȡ��" << " " << slot << " " << "������Ʒ��"
				<< std::endl;
		}

	}
};


// ��ҷ��÷���
THook(__int64,
	MSSYM_B1QE21onBlockPlacedByPlayerB1AE34VanillaServerGameplayEventListenerB2AAA4UEAAB1QE14AW4EventResultB2AAE10AEAVPlayerB2AAA9AEBVBlockB2AAE12AEBVBlockPosB3AAUA1NB1AA1Z,
	void* _this, Player* pPlayer, const Block* pBlk, const BlockPos* pBlkpos, bool _bool) {
	Log::Player::Block("Event", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), "����", pBlk->getLegacyBlock()->getBlockItemID(), pBlkpos->getPosition());
	return original(_this, pPlayer, pBlk, pBlkpos, _bool);
}



// ����ƻ�����
THook(bool,
	MSSYM_B1QE13playerDestroyB1AE11BlockLegacyB2AAE15UEBAXAEAVPlayerB2AAE12AEBVBlockPosB2AAA9AEBVBlockB3AAAA1Z,
	BlockLegacy* _this, Player* pPlayer, const BlockPos* pBlkpos, Block *pBlk) {
	//auto pPlayer = *reinterpret_cast<Player * *>(reinterpret_cast<VA>(_this) + 8);
	//auto pBlk = pPlayer->getRegion()->getBlock(pBlkpos);
	auto block_id = pBlk->getLegacyBlock()->getBlockItemID();
	bool ret = original(_this, pPlayer, pBlkpos, pBlk);
	if (!ret)
		return ret;
	Log::Player::Block("Event", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), "�ƻ�", block_id, pBlkpos->getPosition());
	return ret;
}


// ��Ҵ�����
THook(void,
	MSSYM_B1QA9startOpenB1AE15ChestBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	void* _this, Player* pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	Log::Player::Interaction("Event", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), "����", "����", pBlkpos->getPosition());
	original(_this, pPlayer);
}
// ��ҹر�����
THook(__int64,
	MSSYM_B1QA8stopOpenB1AE15ChestBlockActorB2AAE15UEAAXAEAVPlayerB3AAAA1Z,
	void* _this, Player * pPlayer) {
	auto real_this = reinterpret_cast<void*>(reinterpret_cast<VA>(_this) - 248);
	auto pBlkpos = reinterpret_cast<BlockActor*>(real_this)->getPosition();
	Log::Player::Interaction("Event", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), "�ر�", "����", pBlkpos->getPosition());
	return original(_this, pPlayer);
}

THook(void,
	MSSYM_B1QA7setSlotB1AE26LevelContainerManagerModelB2AAE28UEAAXHAEBUContainerItemStackB3AAAA1Z,
	LevelContainerManagerModel * _this, int a2, ContainerItemStack * a3) {
	auto slot = a2;
	auto pItemStack = a3;
	auto id = pItemStack->getId();
	auto size = pItemStack->getStackSize();
	auto pPlayer = _this->getPlayer();
	if (id >= 0) {
		std::string object_name;
		pItemStack->getName(object_name);
		if (size == 0) {
			Log::Player::Container_Out("Event", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), slot);
		} else
			Log::Player::Container_In("Event", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), slot, size, object_name);
	} else
		Log::Player::Error("Warning", pPlayer->getNameTag()->c_str(), pPlayer->getDimension(), "ʹ����δ֪���飡");
	original(_this, a2, a3);
}

// ���������������Ǳ�Ҫ�ģ������ʹ�ã�Ҳ���Բ�ʹ�á�
void mod_init() {
	// �˴���дģ�����ʱ��Ĳ���
	std::cout << "{��ز���Ѽ��ء�" << std::endl;
}
void mod_exit() {
	// �˴���дģ��ж��ʱ��Ĳ���
}


/*
 UTF-8 ת GBK
 */
static std::string UTF8ToGBK(const char* strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);

	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;

	return strTemp;
}
