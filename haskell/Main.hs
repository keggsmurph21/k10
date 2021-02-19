{-# LANGUAGE OverloadedStrings #-}
{-# OPTIONS_GHC -fwarn-incomplete-patterns #-}
{-# OPTIONS_GHC -fwarn-incomplete-uni-patterns #-}
{-# OPTIONS_GHC -fwarn-incomplete-record-updates #-}

import           Data.List
import           Data.Map  (Map)
import qualified Data.Map  as Map

data PlayerState = WaitForTurn deriving Show

data PlayerId = PlayerId String deriving Show
data Player = Player { pid :: PlayerId
                     , pstate :: PlayerState
                     , pubVPs :: Int
                     , privVPs :: Int
                     , hand :: ResourceCounts
                     , tradeRates :: ResourceCounts
                     , playedDCs :: DevCardCounts
                     , playableDCs :: DevCardCounts
                     , unplayableDCs :: DevCardCounts
                     , hasDeclined :: Bool
                     , toDiscard :: Int
                     } deriving Show

newPlayer :: PlayerId -> Player
newPlayer pid = Player { pid = pid
                       , pstate = WaitForTurn
                       , pubVPs = 0
                       , privVPs = 0
                       , hand = Map.empty
                       , tradeRates = Map.fromList [ (Wheat, 4)
                                                   , (Sheep, 4)
                                                   , (Brick, 4)
                                                   , (Ore, 4)
                                                   , (Wood, 4)
                                                   ]
                       , playedDCs = Map.empty
                       , playableDCs = Map.empty
                       , unplayableDCs = Map.empty
                       , hasDeclined = False
                       , toDiscard = 0
                       }

collectResource :: Resource -> Player -> Player
collectResource r p = p { hand = incrKey r $ hand p }

collectDevCard :: DevCard -> Player -> Player
collectDevCard dc p = p { unplayableDCs = incrKey dc $ unplayableDCs p }

armySize :: Player -> Int
armySize p = sum $ map countKnights $ Map.toList $ playedDCs p
    where countKnights (Knight, count) = count
          countKnights _ = 0

-- Map helpers

addValueAtKey :: Ord k => Num a => a -> k -> Map k a -> Map k a
addValueAtKey delta k m = Map.insertWith (+) k delta m

incrKey :: Ord k => Num a => k -> Map k a -> Map k a
incrKey = addValueAtKey 1

decrKey :: Ord k => Num a => k -> Map k a -> Map k a
decrKey = addValueAtKey (-1)

data Resource = Wheat | Sheep | Brick | Ore | Wood deriving (Eq, Ord, Show)
type ResourceCounts = Map Resource Int

data HexResource = Resource | Ocean | Desert deriving Show
data HexCoords = HexCoords Int Int deriving Show
data Hex = Hex HexCoords HexResource deriving Show

data Road = Road deriving Show

data Junction = Junction deriving Show

-- If [Player] is None, then this is a trade with the bank!
data Trade = Trade { giving :: ResourceCounts
                   , getting :: ResourceCounts
                   , offeredTo :: Maybe [Player]
                   } deriving Show

data DevCard = Knight
             | VictoryPoint
             | Monopoly
             | RoadBuilding
             | YearOfPlenty
             deriving (Eq, Ord, Show)
type DevCardCounts = Map DevCard Int

data Move = AcceptTrade
          | BuildCity Junction
          | BuildSettlement Junction
          | BuildRoad Road
          | BuyDevCard
          | CancelTrade
          | ChooseInitialResources Resource Resource
          | DeclineTrade
          | Discard [Resource]
          | EndTurn
          | MoveRobber Hex
          | OfferTrade Trade
          | PlayKnight Hex
          | PlayVictoryPoint
          | PlayMonopoly Resource
          | PlayRoadBuilding Road Road
          | PlayYearOfPlenty Resource Resource
          | RollDice (Maybe Int)
          | Steal Player
          deriving Show

data Game = Game { players :: [Player]
                 , currentPlayerIndex :: Int
                 } deriving Show

newGame :: [PlayerId] -> Game
newGame [] = error "You can't have 0 players!"
newGame pids = Game { players = map newPlayer pids
                    , currentPlayerIndex = 0
                    }

currentPlayer :: Game -> Player
currentPlayer g = players g !! currentPlayerIndex g

makeMove :: Game -> Player -> Move -> Game
makeMove g p m = error "Not implemented: makeMove"
