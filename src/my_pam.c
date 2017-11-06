#define PAM_SM_ACCOUNT
#define PAM_SM_AUTH
#define PAM_SM_PASSWORD
#define PAM_SM_SESSION

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

void
create_container(const struct passwd *pwd, const char *containerName,
		 const char *password, const char *user_name) {
  char          cmd[128];

  /* Creating an encrypted disk image */
  snprintf(cmd, 128, "dd if=/dev/zero of=%s/%s bs=1 count=0 seek=10G", pwd->pw_dir, containerName);
  system(cmd);
  snprintf(cmd, 128, "echo %s | cryptsetup luksFormat %s/%s", password, pwd->pw_dir,containerName);
  system(cmd);
  /* Creating the mounting folder */
  snprintf(cmd, 128, "mkdir %s/secure_data-rw", pwd->pw_dir);
  system(cmd);
  /*Open and mounting the disk image*/
  snprintf(cmd, 128, "echo %s | cryptsetup luksOpen %s/%s %s", password,
	   pwd->pw_dir, containerName, containerName);
  system(cmd);
  snprintf(cmd, 128, "mkfs.ext4 /dev/mapper/%s", containerName);
  system(cmd);
  snprintf(cmd, 128, "mount /dev/mapper/%s %s/secure_data-rw", containerName, pwd->pw_dir);
  system(cmd);
  snprintf(cmd, 128, "chown %s:%s -R %s/secure_data-rw", user_name, user_name, pwd->pw_dir);
  system(cmd);
}

void
mount_container(const struct passwd *pwd, const char *containerName,
		const char *password, const char *user_name) {
  char          cmd[128];

  /* Creating the mounting folder */
  system("mkdir secure_data-rw");
  /*Open and mounting the disk image*/
  snprintf(cmd, 128, "echo %s | cryptsetup luksOpen %s/%s %s", password,
	   pwd->pw_dir, containerName, containerName);
  system(cmd);
  snprintf(cmd, 128, "mount /dev/mapper/%s %s/secure_data-rw", containerName, pwd->pw_dir);
  system(cmd);
  snprintf(cmd, 128, "chown %s:%s -R %s/secure_data-rw", user_name, user_name, pwd->pw_dir);
  system(cmd);
}

/* PAM entry point for session creation */
int
pam_sm_open_session(pam_handle_t *pamh, int flags, int ac, const char **av) {
  const char    *user_name = NULL;
  struct passwd	*pwd = NULL;
  struct passwd result;
  char		buffer[1024];
  char		*password;
  char		containerName[256];
  char		to_check[256];

  (void)flags;
  (void)ac;
  (void)av;
  /* Getting user name */
  if (pam_get_user(pamh, &user_name, NULL) != PAM_SUCCESS || user_name == NULL) {
    return (PAM_IGNORE);
  }
  /* Check if user is root */
  if (!strcmp(user_name, strdup("root"))) {
    return (PAM_IGNORE);
  }
  /* Getting user password */
  pam_get_data(pamh, "user_password", (const void **)(&password));
  /* Getting home path */
  if (getpwnam_r(user_name, &result, buffer, sizeof(buffer), &pwd) != 0 || pwd == NULL
      || pwd->pw_dir == NULL || pwd->pw_dir[0] != '/') {
    return (PAM_IGNORE);
  }
  snprintf(containerName, 256, "%sContainer", user_name);
  snprintf(to_check, 256, "%s/%s", pwd->pw_dir, containerName);
  /* Checking if the container exist */
  if (access(to_check, F_OK) == -1) {
    create_container(pwd, containerName, password, user_name);
  } else {
    mount_container(pwd, containerName, password, user_name);
  }
  return (PAM_IGNORE);
}

/* PAM entry point for session cleanup */
int
pam_sm_close_session(pam_handle_t *pamh, int flags, int ac, const char **av) {
  const char    *user_name = NULL;
  struct passwd	*pwd = NULL;
  struct passwd result;
  char		buffer[1024];
  char		cmd[128];

  (void)flags;
  (void)ac;
  (void)av;
  if (pam_get_user(pamh, &user_name, NULL) != PAM_SUCCESS || user_name == NULL) {
    return (PAM_IGNORE);
  }
  /* Check if user is root */
  if (!strcmp(user_name, strdup("root"))) {
    return (PAM_IGNORE);
  }

  if (getpwnam_r(user_name, &result, buffer, sizeof(buffer), &pwd) != 0 || pwd == NULL
      || pwd->pw_dir == NULL || pwd->pw_dir[0] != '/') {
    return (PAM_IGNORE);
  }
  snprintf(cmd, 128, "umount %s/secure_data-rw && cryptsetup luksClose %sContainer", pwd->pw_dir, user_name);
  system(cmd);
  return (PAM_IGNORE);
}

void
do_nothing(pam_handle_t *pamh, void *data, int error_status) {
  (void)pamh;
  (void)data;
  (void)error_status;
}

/* PAM entry point for authentication verification */
int
pam_sm_authenticate(pam_handle_t *pamh, int flags, int ac, const char **av) {
  char		*password;
  const char    *user_name = NULL;

  (void)flags;
  (void)ac;
  (void)av;
  /* Getting username */
  if (pam_get_user(pamh, &user_name, NULL) != PAM_SUCCESS || user_name == NULL) {
    return (PAM_IGNORE);
  }
  /* Check if user is root */
  if (!strcmp(user_name, strdup("root"))) {
    return (PAM_IGNORE);
  }
  /* Getting user's password */
  if (pam_get_item(pamh, PAM_AUTHTOK, (const void **)(&password)) != PAM_SUCCESS) {
    return (PAM_IGNORE);
  }
  /* Storing the password */
  pam_set_data(pamh, "user_password", strdup(password), &do_nothing);
  return (PAM_IGNORE);
}

/* PAM entry point for authentication token (password) changes */
int
pam_sm_chauthtok(pam_handle_t *pamh, int flags, int ac, const char **av) {
  (void)pamh;
  (void)flags;
  (void)ac;
  (void)av;
  return(PAM_IGNORE);
}
