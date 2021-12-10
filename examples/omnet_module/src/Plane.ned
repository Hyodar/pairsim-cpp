
package avens_pairsim;

import avens_pairsim.PlaneMobility;

module Plane {
    parameters:
        @networkNode();
        @display("i=device/pocketpc");
        @class(Plane);
    submodules:
        mobility: PlaneMobility {
            @display("p=100,100;is=s");
        }
}
