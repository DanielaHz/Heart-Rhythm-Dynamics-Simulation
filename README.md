# Heart Rhythm Dynamics Simulation Using a Three-Coupled Oscillator Model

![img](/images/heart.png)

The present project aims to simulate the dynamics of a human heart using the three-coupled oscillator model proposed in the paper of the same name. Since the heart is a soft body, I will use the JellyEngine as the boilerplate for this assessment.

## Execution 

Build the project
```
cmake -DCMAKE_BUILD_TYPE=Release -B out .
```

Execute the project
```
mkdir build 
cd build 
cmake --build
./heartsimulation
```

## Resources 

- https://github.com/Rafapp/jellyengine/blob/main/build.bat
- http://mecanon.coppe.ufrj.br/wp-content/uploads/2017/11/Heart-Rhythms_ChaosSolFractals09.pdf
- https://uvadoc.uva.es/bitstream/handle/10324/36037/TFG-G3546.pdf?sequence=1&isAllowed=y
- https://www.youtube.com/watch?v=j-8zfz-E73c&t=2269s&ab_channel=InsideLivermoreLab
- https://www.fab.com/listings/30c310b0-8317-4e92-a9dd-42a55046da6b
