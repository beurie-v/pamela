--- INSTALLATION ---
Follow the instructions to make a full install of our PAM module.

Make sure you have root access before trying the installation

Then run:

$> make

This is mandatory before trying to install the module as it will install the dependencies needed for compilation and then compile the module.



Once the module is compiled you can run:

$> make install

This will install and configure PAM so that it loads our custom module and uses it.



Once this is done, you can verify the installation is complete by running:

$> make check



If you want to uninstall our module from PAM run:

$> make uninstall

And you can use

$> make check

again to make sure the module is fully uninstalled.
