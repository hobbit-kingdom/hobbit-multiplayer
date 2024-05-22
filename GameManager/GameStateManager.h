#pragma once
#include "MemoryAccess.h"
#include "GamePacket.h"
#include "ClientEntity.h"

#include "MainPlayer.h"
#include "OtherPlayer.h"

#include <vector>
//0x00760864: loading layers

class GameManager
{
private:
    // All derived classes
    static std::vector<ClientEntity*> clientEntities;
    
    
    static uint32_t gameState;
    static uint32_t currentLevel;

    static void readGameState()
    {
        gameState = MemoryAccess::readData(0x00762B58); // 0x00762B58: game state
    }
    static void readGameLevel()
    {
        currentLevel = MemoryAccess::readData(0x00762B5C);  // 00762B5C: current level
    }
public:
    static void Start()
    {
        // check if the game is open
        std::string s;
        while (!checkGameOpen())
        {
            std::cout << "Press [y] when the Hobbit_2003 is open: ";
            std::cin >> s;
        }
        //
        readInstanices();
    }
    static void Update()
    {
        readInstanices();

        // handle game states
        {
            static uint32_t lastState;
            static uint32_t currentState;

            currentState = getGameState();

            // 0xA: open level
            if (lastState != currentState && currentState == 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->EnterNewLevel();
                }
            }
            // exit level
            if (lastState == 0xA && currentState != 0xA)
            {
                // call enterNewLevel for all classes
                for (ClientEntity* e : clientEntities)
                {
                    e->ExitLevel();
                }
            }
            lastState = currentState;
        }

        // handle Update event
        {
            for (ClientEntity* e : clientEntities)
            {
                e->Update();
            }
        }

    }

    static void readPackets(std::vector<uint32_t>& packets, uint32_t playerIndex) 
    {
        // convert packets into GamePackets
        std::vector<GamePacket> gamePackets;
        gamePackets = GamePacket::packetsToGamePackets(packets);
        
        // read the GamePackets
        for (GamePacket gamePacket : gamePackets)
        {
            for (uint32_t reader : gamePacket.getReadersIndexes())
            {
                clientEntities[reader]->ReadPackets(gamePacket, playerIndex);
            }
        }
    }
    static std::vector<uint32_t> setPackets()
    {
        std::vector<uint32_t> packets;      // packets to send
        std::vector<uint32_t> entityPackets;// entity packets
        std::vector<GamePacket> gamePackets;// packeof the game

        // get packets from all entities
        for (ClientEntity* e : clientEntities)
        {
            gamePackets.push_back(e->SetPackets());
        }

        // end of packets
        gamePackets.back().endPacket();

        return packets;
    }


    static bool checkGameOpen()
    {
        MemoryAccess::setExecutableName("Meridian.exe");
        return MemoryAccess::readProcess();
    }
    static void readInstanices()
    {
        readGameState();
        readGameLevel();
    }
    

    static uint32_t getGameState()
    {
        return gameState;
    }
    static uint32_t getGameLevel()
    {
        return currentLevel;
    }
};
