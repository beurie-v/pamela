--- INSTALLATION ---

Follow the instructions to make a full install of our PAM module.

Make sure you have root access before trying the installation

This rule will install the dependencies needed by the module.
It will then compile the module.
When the module is compiled, this rule will install it and configure it.

$> make install


You can verify the installation is complete by running:

$> make check



If you want to uninstall our module from PAM run:

$> make uninstall

And you can use

$> make check

again to make sure the module is fully uninstalled.
