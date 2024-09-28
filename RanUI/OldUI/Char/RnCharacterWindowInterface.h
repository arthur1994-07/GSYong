
#ifndef _RnCharacterWindowInterface_h_
#define _RnCharacterWindowInterface_h_

class RnCharacterWindowInterface
{

public:
	virtual	void UpdateTitleData() = 0;

	virtual void DeleteUIMeshData() {}

	virtual void SetStorageItemToVehicleTapWindow(const std::vector<DWORD>& vehicleIDVec) {}

	virtual void SetVisibleCharacterAdditionalWindow( bool bVisible ) {}

	virtual void SetArmSwapTabButton() {}

};

#endif
