#ifndef CAISHEN_MCCFR_CFR_DOUDIZHU_PLAYER_H_
#define CAISHEN_MCCFR_CFR_DOUDIZHU_PLAYER_H_

#include "doudizhu/doudizhu_game.h"
#include "mccfr.h"

#include <Eigen/Dense>

namespace caishen
{
namespace cfr
{

class InformationSetStore
{
public:
    virtual void getInformationSet(const std::string &key, InformationSet &is) = 0;
    virtual void putInformationSet(const std::string &key, InformationSet &is) = 0;
};

class CFRDoudizhuPlayer
{
public:
    CFRDoudizhuPlayer(int playerId) : _playerId(playerId) {}
    virtual ~CFRDoudizhuPlayer() {}

    virtual void getLegalAbstractActions(caishen::doudizhu::Game &game, Eigen::Ref<AbstractActionSpace> legalActions) = 0;
    virtual void abstractAction2ActionCards(caishen::doudizhu::Game &game, int abstractActionId, std::vector<std::unique_ptr<caishen::doudizhu::Action>> & actions) = 0;

    virtual int pickAbstractAction(caishen::doudizhu::Game &game) = 0;

protected:
    int _playerId;
};

} // namespace cfr
} // namespace caishen

#endif