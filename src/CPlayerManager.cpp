
#include "CPlayerManager.h"

CPlayerManager gPlayerManager;

CPlayerManager::CPlayerManager()
{
}

void CPlayerManager::init()
{
	playerModeSwitch = new F1_ConVar<Switch>( "Player List", false );

	gInts.EventManager->AddListener( this, "player_disconnect", false );
	gInts.EventManager->AddListener( this, "player_connect_client", true );
}

DWORD CPlayerManager::getColorForPlayer( int index )
{
	//_TRY
	//{

	//	auto info = gInts.Engine->GetPlayerInfo( index );

	//	if( playerArray[ info.userID ].isTarget == true )
	//	{
	//		return COLORCODE( 170, 0, 255, 255 );
	//	}
	//	else
	//	{
	//		if( playerArray[ info.userID ].mode != NULL )
	//			switch( playerArray[ info.userID ].mode->getValue() )
	//			{
	//			case playerMode::Normal:
	//				return 0xFFFFFFFF;
	//				break;
	//			case playerMode::Friend:
	//				return 0x00FF32FF;
	//				break;
	//			case playerMode::Rage:
	//				return 0xFF0000FF;
	//				break;
	//			}
	//	}
	//}
	//_CATCH
	//{

	//}

	// return black by default
	return 0xFFFFFFFF;
}

CPlayerManager::playerMode CPlayerManager::getModeForPlayer( int index )
{
	//_TRY
	//{
	//	player_info_t info;
	//	if( gInts.Engine->GetPlayerInfo( index, &info ) )
	//	{
	//		return playerArray[ info.userID ].mode->getValue();
	//	} 
	//}
	//_CATCH
	//{ 
	//}

	return playerMode::Normal;
}

void CPlayerManager::menuUpdate( F1_IConVar **menuArray, int &currIndex )
{
	//menuArray[ currIndex++ ] = playerModeSwitch;

	//if( playerModeSwitch->getValue() )
	//{
	//	for(auto &p : playerArray )
	//	{
	//		if( p.second.isValid == true && p.second.mode != NULL )
	//		{
	//			assert( p.second.mode == NULL );
	//			menuArray[ currIndex++ ] = p.second.mode;
	//		}
	//	}
	//}
}

bool CPlayerManager::paint( )
{
	return false;
}

void CPlayerManager::setTarget( int index )
{
	//playerMutex.lock();
	//if( index != -1 && index > 1 )
	//{
	//	player_info_t info;

	//	if( gInts.Engine->GetPlayerInfo( index, &info ) )
	//	{
	//		playerArray[ info.userID ].isTarget = true;

	//		if( lastTargetIndex != -1 )
	//		{
	//			playerArray[ lastTargetIndex ].isTarget = false;
	//		}
	//	}
	//}
	//playerMutex.unlock();
}

void CPlayerManager::FireGameEvent( IGameEvent *event )
{
	//_TRY
	//{
	//	if( gLocalPlayerVars.info.userID == 0 )
	//		return;

	//	playerMutex.lock();


	//	if( !strcmp( event->GetName(), "player_disconnect" ) )
	//	{
	//		Log::Console( "--------------------------> PLAYER DISCONNECT" );

	//		auto uid = event->GetInt( "userid" );

	//		Log::Console( "uid=%i", uid );

	//		if( playerArray[ uid ].mode != NULL )
	//		{
	//			delete playerArray[ uid ].mode;
	//			playerArray[ uid ].mode = NULL;

	//			playerArray[ uid ].isValid = false;
	//			playerArray[ uid ].uid = -1;
	//		}

	//	}
	//	else if( !strcmp( event->GetName(), "player_connect_client" ) )
	//	{
	//		Log::Console( "--------------------------> PLAYER CONNECT" );

	//		auto name = event->GetString( "name", "<null>" );
	//		auto uid = event->GetInt( "userid" );

	//		Log::Console( "uid=%i", uid );


	//		if( playerArray[ uid ].mode != NULL )
	//		{
	//			delete playerArray[ uid ].mode;
	//			playerArray[ uid ].mode = NULL;
	//		}


	//		char newName[ 35 ] = " - ";

	//		strcat_s( newName, name );
	//		playerArray[ uid ].mode = new F1_ConVar<Enum<playerMode>>( name, playerStateEnum, playerMode::Normal, playerMode::Rage );

	//		playerArray[ uid ].isValid = true;
	//	}
	//	playerMutex.unlock();
	//}
	//_CATCH
	//{
	//	playerMutex.unlock();
	//}
}
