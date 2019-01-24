#include "NlPxMaterial.h"
#include "NlPhysicsDevice.h"

//=================================================================================
NlPxMaterial::NlPxMaterial( NlPhysicsDevice* device )
{
	assert( device );
	m_scene = device->GetScene();

	m_mtrl = NULL;

}//---------------------------------------------------------------------------------

NlPxMaterial::~NlPxMaterial(void)
{
}
//---------------------------------------------------------------------------------
void NlPxMaterial::init( NlPxMtrlParam& param )
{
	assert( m_scene );

	// create a new PhysX material
	NxMaterialDesc desc;
	desc.restitution = param._bounciness;
	desc.staticFriction = param._staticFriction;
	desc.dynamicFriction = param._dynamicFriction;
	
	switch(param._BounceCombine)
	{
	case ePMComb_Average:
		desc.restitutionCombineMode = NX_CM_AVERAGE;
		break;
	case ePMComb_MIN:
		desc.restitutionCombineMode = NX_CM_MIN;
		break;
	case ePMComb_MAX:
		desc.restitutionCombineMode = NX_CM_MAX;
		break;
	case ePMComb_MULTIPLU:
		desc.restitutionCombineMode = NX_CM_MULTIPLY;
		break;
	}

	switch(param._frictionCombine)
	{
	case ePMComb_Average:
		desc.frictionCombineMode = NX_CM_AVERAGE;
		break;
	case ePMComb_MIN:
		desc.frictionCombineMode = NX_CM_MIN;
		break;
	case ePMComb_MAX:
		desc.frictionCombineMode = NX_CM_MAX;
		break;
	case ePMComb_MULTIPLU:
		desc.frictionCombineMode = NX_CM_MULTIPLY;
		break;
	}

	m_mtrl = m_scene->createMaterial( desc );
	assert( m_mtrl );
	m_index = m_mtrl->getMaterialIndex();

}

//=================================================================================
NlPxMtrlManager* NlPxMtrlManager::instance = NULL;
uint16 NlPxMtrlManager::curMaxIndex = 0;

NlPxMtrlManager::~NlPxMtrlManager()
{
	ClearUp();
}
//---------------------------------------------------------------------------------
void NlPxMtrlManager::Create()
{
	Destroy();
	instance = new NlPxMtrlManager;
	assert( instance );

}
//---------------------------------------------------------------------------------
NlPxMtrlManager& NlPxMtrlManager::GetInstance()
{
	assert( instance );
	return *instance;

}
//---------------------------------------------------------------------------------
void NlPxMtrlManager::Destroy()
{
	if (instance)
	{
		delete instance;
		instance = NULL;
	}

}
//---------------------------------------------------------------------------------
uint16 NlPxMtrlManager::CreateMaterial( NlPhysicsDevice* device, NlPxMtrlParam& param, const char* name )
{
	NlPxMaterial* newMtrl = new NlPxMaterial( device );
	assert( newMtrl );
	newMtrl->m_name = std::string(name);
	newMtrl->init( param );
	
	m_mtrls[newMtrl->m_name] = newMtrl;
	return newMtrl->m_index;

}
//---------------------------------------------------------------------------------
NlPxMaterial* NlPxMtrlManager::GetMaterialByName( const char* name )
{
	NlPxMtrlMapper::iterator it;

	return m_mtrls[name];

}
//---------------------------------------------------------------------------------
void NlPxMtrlManager::ClearUp()
{
	NlPxMtrlMapper::iterator it;
	for (it = m_mtrls.begin(); it != m_mtrls.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = NULL;
		}
	}

	m_mtrls.clear();

}