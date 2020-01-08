<h1>Haptics Sailing</h1>

![Haptic sailing screenshot](Screenshot.png)

Sailing simulator made using OpenGL and the chai3D haptics library.

<h2>Features</h2>

<h3>Haptics</h3>
The boat is controlled using two Novint Falcon haptic devices. One device controls the sheet (Rope attached to the boom) and the other controls the rudder.

<h3>Bouyancy</h3>
The boat is modeled as a rigid body with boyancy calculated by measuring the difference between the bottom of the boat and the top of the water (to a maximum of the height of the top of the boat). This is calculated by making a low resolution rendering of a simplified hull, and comparing the pixel values between the result for the bottom of the ship, top of the ship, and water surface.

<h3>Sail</h3>
The sail of the ship is modeled with a mass spring model using the method described in Deformation constraints in a mass-spring model to describe rigid cloth behaviour by Xavier Provot to calculate interactions with wind. The force applied to the attached masses to the ship are transfered to their respective components.
