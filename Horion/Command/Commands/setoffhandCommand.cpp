#include "setoffhandCommand.h"

#include <string>
#include <vector>

setoffhandCommand::setoffhandCommand() : IMCCommand("setoffhand", "Puts any item into offhand", "<ItemName> <count> <itemData>") {
	registerAlias("soh");
}

setoffhandCommand::~setoffhandCommand() {
}

bool setoffhandCommand::execute(std::vector<std::string>* args) {
	assertTrue(args->size() > 2);

	int itemId = 0;
	char count = static_cast<char>(assertInt(args->at(2)));
	char itemData = 0;
	if (args->size() > 3)
		itemData = static_cast<char>(assertInt(args->at(3)));

	try {
		itemId = std::stoi(args->at(1));
	} catch (const std::invalid_argument&) {
	}

	C_Inventory* inv = g_Data.getLocalPlayer()->getSupplies()->inventory;
	C_ItemStack* yot = nullptr;
	auto transactionManager = g_Data.getLocalPlayer()->getTransactionManager();

	using getItemFromId_t = C_Item***(__fastcall*)(void*, int itemID);
	static getItemFromId_t getItemFromId = reinterpret_cast<getItemFromId_t>(FindSignature("40 53 48 83 EC ?? 48 8B D9 66 85 D2 0F 84 ?? ?? ?? ?? 44 0F BF C2 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 44 89 44 24 ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? ?? 41 0F B6 D0 48 33 D0 0F B6 44 24 ??"));

	using ItemRegistry__lookupByName_t = C_Item***(__fastcall*)(void*, void*, TextHolder);
	static ItemRegistry__lookupByName_t ItemRegistry__lookupByNameF = reinterpret_cast<ItemRegistry__lookupByName_t>(FindSignature("40 55 56 57 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 C7 45 ? ? ? ? ? 48 89 9C 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 48 48 8B F1 48 89 4D 80 45 33 FF 44 89 7C 24 ? 4D 39 78 10 75 23 "));

	if (itemId == 0) {
		void* ItemPtr = malloc(0x8);
		void* idk = malloc(0x0);
		C_Item*** cStack = ItemRegistry__lookupByNameF(ItemPtr, idk, TextHolder(args->at(1)));
		if (*cStack == nullptr) {
			clientMessageF("%sInvalid item name!", RED);
			return true;
		}
		yot = new C_ItemStack(***cStack, count, itemData);
		free(ItemPtr);
		free(idk);
	} else {
		void* ItemPtr = malloc(0x8);
		C_Item*** cStack = getItemFromId(ItemPtr, itemId);
		if (**cStack == NULL) {
			clientMessageF("%sInvalid item ID!", RED);
			return true;
		}
		yot = new C_ItemStack(***cStack, count, itemData);
		free(ItemPtr);
	}

	if (yot != nullptr)
		yot->count = count;

	g_Data.getLocalPlayer()->setOffhandSlot(yot);
	clientMessageF("%sSet item as offhand!", BLUE);
	return true;
}
