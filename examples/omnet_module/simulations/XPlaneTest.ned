
package pairsim.simulations;

import pairsim.PairsimClient;

network XPlaneTest {
parameters:
    @display("bgb=650,500;bgg=100,1,grey95");
    @figure[title](type=label; pos=0,-1; anchor=sw; color=darkblue);
submodules:
    client: PairsimClient {
        @display("p=580,200");
    }
}
