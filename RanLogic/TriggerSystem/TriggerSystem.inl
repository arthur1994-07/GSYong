// TAG_InitClientTriggerSystem_6
template<class T>
void TriggerSystem::CreateScript(const TCHAR* pFile)
{
	if (FindScript(pFile) == false)
	{
		SpTriggerScript script(new T);

		if (script)
		{
			script->Init();
			script->LoadScript(pFile);

			InsertTrigger(pFile, script);
		}
	}
}