RM		= rm -f

CC		= gcc

LINK		= ld

SRC		= ./src/my_pam.c

NAME		= my_pam.so

SOPATH		= /lib/security/

AUTHPATH	= /etc/pam.d/common-auth

PASSPATH	= /etc/pam.d/common-password

SESSPATH	= /etc/pam.d/common-session

CMDAUTHLINE	= auth required

CMDPASSLINE	= password required

CMDSESSLINE	= session required

GETPAMPKGS	= apt-get install libpam0g-dev

OBJNAME		= ./src/my_pam.o

LDFLAGS		+= -x --shared

CFLAGS		+= -W -Wall -Wextra -Werror -I./include -fPIC

all:	$(NAME)

$(NAME):
	@apt-get -qq install gcc
	@apt-get -qq install libpam0g-dev
	@apt-get -qq install cryptsetup
	@apt-get -qq install libcryptsetup-dev 
	$(CC) -c $(SRC) $(CFLAGS) -o $(OBJNAME)
	$(LINK) $(LDFLAGS) -o $(NAME) $(OBJNAME)

clean:
	$(RM) $(OBJNAME)
	$(RM) $(NAME)

re:	clean all

install:
	@printf "\033[0;33m===== Installing module $(NAME) =====\n\033[0m"
	@printf "\033[0;1m--- Adding $(NAME) module ---\n\033[0m"
	@[ -d $(SOPATH) ] || mkdir $(SOPATH)
	cp $(NAME) $(SOPATH)
	chown root:root $(SOPATH)$(NAME)
	chmod 755 $(SOPATH)$(NAME)
	@printf "\033[0;1m--- Configuring PAM ---\n\033[0m"
	@(grep -q $(NAME) $(AUTHPATH)) || echo >> $(AUTHPATH)
	@(grep -q $(NAME) $(AUTHPATH)) || echo $(CMDAUTHLINE) $(NAME) >> $(AUTHPATH)
	@(grep -q $(NAME) $(PASSPATH)) || echo >> $(PASSPATH)
	@(grep -q $(NAME) $(PASSPATH)) || echo $(CMDPASSLINE) $(NAME) >> $(PASSPATH)
	@(grep -q $(NAME) $(SESSPATH)) || echo >> $(SESSPATH)
	@(grep -q $(NAME) $(SESSPATH)) || echo $(CMDSESSLINE) $(NAME) >> $(SESSPATH)
	@printf "\033[0;33m===== Installation complete =====\n\033[0m"

uninstall:
	@printf "\033[0;33m===== Uninstalling module $(NAME) =====\n\033[0m"
	@printf "\033[0;1m--- Removing $(NAME) module ---\n\033[0m"
	@[ -f $(SOPATH)$(NAME) ] && $(RM) $(SOPATH)$(NAME) || echo "Module $(NAME) already removed"
	@printf "\033[0;1m--- Reconfiguring PAM ---\n\033[0m"
	@grep -v $(NAME) $(AUTHPATH) > $(AUTHPATH).tmp
	@mv $(AUTHPATH).tmp $(AUTHPATH)
	@grep -v $(NAME) $(PASSPATH) > $(PASSPATH).tmp
	@mv $(PASSPATH).tmp $(PASSPATH)
	@grep -v $(NAME) $(SESSPATH) > $(SESSPATH).tmp
	@mv $(SESSPATH).tmp $(SESSPATH)
	@printf "\033[0;33m===== Uninstallation complete =====\n\033[0m"

test:
	@printf "\033[0;33mTests not yet implemented\n\033[0m"

check:
	@printf "\033[0;33m===== Start of checks =====\n\033[0m"
	@printf "\033[0;1m--- Checking if $(NAME) module is installed ---\n\033[0m"
	@[ -f $(SOPATH)$(NAME) ] && echo $(NAME) "module is \033[0;32minstalled\033[0m" || echo $(NAME) "module is \033[0;31mnot installed\033[0m"
	@printf "\n\033[0;1m--- Checking if $(NAME) module is configured ---\n\033[0m"
	@(grep -q $(NAME) $(AUTHPATH)) && echo "common-auth configuration \033[0;32mOK\033[0m" || echo "common-auth configuration \033[0;31mKO\033[0m"
	@(grep -q  $(NAME) $(PASSPATH)) && echo "common-password configuration \033[0;32mOK\033[0m" || echo "common-password configuration \033[0;31mKO\033[0m"
	@(grep -q $(NAME) $(SESSPATH)) && echo "common-session configuration \033[0;32mOK\033[0m" || echo "common-session configuration \033[0;31mKO\033[0m"
	@printf "\033[0;33m===== End of checks =====\n\033[0m"
