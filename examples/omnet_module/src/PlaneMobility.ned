
package avens_pairsim;

import inet.mobility.base.MovingMobilityBase;

simple PlaneMobility extends MovingMobilityBase
{
    parameters:
        double initialX @unit(m) = default(0m);
        double initialY @unit(m) = default(0m);
        double initialZ @unit(m) = default(0m);
        bool initFromDisplayString = default(true);
        double speed @unit(mps) = default(0mps);
        double initialMovementHeading @unit(deg) = default(0deg);
        double initialMovementElevation @unit(deg) = default(0deg);
        
        constraintAreaMaxX @unit(m) = default(650m);
        constraintAreaMinX @unit(m) = default(0m);
        constraintAreaMaxY @unit(m) = default(650m);
        constraintAreaMinY @unit(m) = default(0m);
        constraintAreaMaxZ @unit(m) = default(650m);
        constraintAreaMinZ @unit(m) = default(0m);
        
        @class(PlaneMobility);
}
