/*

 *
 */

/* $Id: hash.c,v 1.14.4.4 2002/07/30 03:59:26 jnelson Exp $*/

#include "boa.h"
#include "parse.h"

/*
 * There are two hash tables used, each with a key/value pair
 * stored in a hash_struct.  They are:
 *
 * mime_hashtable:
 *     key = file extension
 *   value = mime type
 *
 * passwd_hashtable:
 *     key = username
 *   value = home directory
 *
 */

struct _hash_struct_ {
	char *key;
	char *value;
	struct _hash_struct_ *next;
};

typedef struct _hash_struct_ hash_struct;

static hash_struct *mime_hashtable[MIME_HASHTABLE_SIZE];
static hash_struct *passwd_hashtable[PASSWD_HASHTABLE_SIZE];

#ifdef WANT_ICKY_HASH
static unsigned four_char_hash(char *buf);
#define boa_hash four_char_hash
#else
#ifdef WANT_SDBM_HASH
static unsigned sdbm_hash(char *str);
#define boa_hash sdbm_hash
#else
static unsigned djb2_hash(char *str);
#define boa_hash djb2_hash
#endif
#endif

#ifdef WANT_ICKY_HASH
static unsigned four_char_hash(char *buf)
{
	unsigned int hash = (buf[0] +
			(buf[1] ? buf[1] : 241 +
			 (buf[2] ? buf[2] : 251 +
			  (buf[3] ? buf[3] : 257))));
#ifdef DEBUG_HASH
	log_error_time();
	fprintf(stderr, "four_char_hash(%s) = %u\n", buf, hash);
#endif
	return hash;
}

/* The next two hashes taken from
 * http://www.cs.yorku.ca/~oz/hash.html
 *
 * In my (admittedly) very brief testing, djb2_hash performed
 * very slightly better than sdbm_hash.
 */

#else
#define MAX_HASH_LENGTH 4
#ifdef WANT_SDBM_HASH
static unsigned sdbm_hash(char *str)
{
	unsigned hash = 0;
	int c;
	short count = MAX_HASH_LENGTH;

#ifdef DEBUG_HASH
	log_error_time();
	fprintf(stderr, "sdbm_hash(%s) = ", str);
#endif

	while ((c = *str++) && count--)
		hash = c + (hash << 6) + (hash << 16) - hash;

#ifdef DEBUG_HASH
	fprintf(stderr, "%u\n", hash);
#endif
	return hash;
}
#else

static unsigned djb2_hash(char *str)
{
	unsigned hash = 5381;
	int c;
	short count = MAX_HASH_LENGTH;

#ifdef DEBUG_HASH
	log_error_time();
	fprintf(stderr, "djb2_hash(%s) = ", str);
#endif

	while ((c = *(str++)) && count--)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

#ifdef DEBUG_HASH
	fprintf(stderr, "%u\n", hash);
#endif
	return hash;
}
#endif
#endif

/*
 * Name: add_mime_type
 * Description: Adds a key/value pair to the mime_hashtable
 */

void add_mime_type(char *extension, char *type)
{
	unsigned int hash;
	hash_struct *current, *next;

	if (!extension)
		return;

	hash = get_mime_hash_value(extension);

	current = mime_hashtable[hash];

	while (current) {
		if (!strcmp(current->key, extension))
			return;         /* don't add extension twice */
		if (current->next)
			current = current->next;
		else
			break;
	}

	/* if here, we need to add a new one */
	next = (hash_struct *) malloc(sizeof (hash_struct));
	if (!next) {
		DIE("malloc of hash_struct failed!");
	}
	next->key = strdup(extension);
	if (!next->key)
		DIE("malloc of hash_struct->key failed!");
	next->value = strdup(type);
	if (!next->value)
		DIE("malloc of hash_struct->value failed!");
	next->next = NULL;

	if (!current) {
		mime_hashtable[hash] = next;
	} else {
		current->next = next;
	}
}

/*
 * Name: get_mime_hash_value
 *
 * Description: adds the ASCII values of the file extension letters
 * and mods by the hashtable size to get the hash value
 */

unsigned get_mime_hash_value(char *extension)
{
	unsigned int hash = 0;

	if (extension == NULL || extension[0] == '\0') {
		/* FIXME */
		log_error_time();
		fprintf(stderr, "Attempt to hash NULL or empty string!\n");
		return 0;
	}

	hash = boa_hash(extension);
	hash %= MIME_HASHTABLE_SIZE;

	return hash;
}

/*
 * Name: get_mime_type
 *
 * Description: Returns the mime type for a supplied filename.
 * Returns default type if not found.
 */

char *get_mime_type(char *filename)
{
	char *extension;
	hash_struct *current;

	unsigned int hash;

	extension = strrchr(filename, '.');
	if (!extension || *extension++ == '\0')
		return default_type;
	hash = get_mime_hash_value(extension);
	//fprintf(stderr,"hash=%d    %s:%d\n",hash,__FILE__,__LINE__);
	current = mime_hashtable[hash];
	
	while (current) {
		//fprintf(stderr,"1111111111111111111111current->key= %s   %s:%d \n",current->key,__FILE__,__LINE__);
		if (!strcmp(current->key, extension)) /* hit */
			return current->value;
		current = current->next;
	}
	return default_type;
}

/*
 * Name: get_homedir_hash_value
 *
 * Description: adds the ASCII values of the username letters
 * and mods by the hashtable size to get the hash value
 */

unsigned get_homedir_hash_value(char *name)
{
	unsigned int hash = 0;

	if (name == NULL || name[0] == '\0') {
		/* FIXME */
		log_error_time();
		fprintf(stderr, "Attempt to hash NULL or empty string!\n");
		return 0;
	}

	hash = boa_hash(name);
	hash %= PASSWD_HASHTABLE_SIZE;

	return hash;
}


/*
 * Name: get_home_dir
 *
 * Description: Returns a point to the supplied user's home directory.
 * Adds to the hashtable if it's not already present.
 *
 */

char *get_home_dir(char *name)
{
	struct passwd *passwdbuf;

	hash_struct *current, *next;
	unsigned int hash;

	/* first check hash table -- if username is less than four characters,
	   just hash to zero (this should be very rare) */

	hash = get_homedir_hash_value(name);

	for(current = passwd_hashtable[hash];current;current = current->next) {
		if (!strcmp(current->key, name)) /* hit */
			return current->value;
		if (!current->next)
			break;
	}

	/* if here, we have to add a new one */

	passwdbuf = getpwnam(name);

	if (!passwdbuf)         /* does not exist */
		return NULL;

	next = (hash_struct *) malloc(sizeof (hash_struct));
	if (!next) {
		WARN("malloc of hash_struct for passwd_hashtable failed!");
		return NULL;
	}

	next->key = strdup(name);
	if (!next->key) {
		WARN("malloc of passwd_hashtable[hash]->key failed!");
		free(next);
		return NULL;
	}
	next->value = strdup(passwdbuf->pw_dir);
	if (!next->value) {
		WARN("malloc of passwd_hashtable[hash]->value failed!");
		free(next->key);
		free(next);
		return NULL;
	}
	next->next = NULL;

	if (!current) {
		passwd_hashtable[hash] = next;
	} else {
		current->next = next;
	}
	return next->value;
}

void dump_mime(void)
{
	int i;
	hash_struct *temp;
	for (i = 0; i < MIME_HASHTABLE_SIZE; ++i) { /* these limits OK? */
		temp = mime_hashtable[i];
		while (temp) {
			hash_struct *temp_next;

			temp_next = temp->next;
			free(temp->key);
			free(temp->value);
			free(temp);

			temp = temp_next;
		}
		mime_hashtable[i] = NULL;
	}
}

void dump_passwd(void)
{
	int i;
	hash_struct *temp;
	for (i = 0; i < PASSWD_HASHTABLE_SIZE; ++i) { /* these limits OK? */
		temp = passwd_hashtable[i];
		while (temp) {
			hash_struct *temp_next;

			temp_next = temp->next;
			free(temp->key);
			free(temp->value);
			free(temp);

			temp = temp_next;
		}
		passwd_hashtable[i] = NULL;
	}
}

void show_hash_stats(void)
{
	int i;
	hash_struct *temp;
	int total = 0;
	int count;

	for (i = 0; i < MIME_HASHTABLE_SIZE; ++i) { /* these limits OK? */
		if (mime_hashtable[i]) {
			count = 0;
			temp = mime_hashtable[i];
			while (temp) {
				temp = temp->next;
				++count;
			}
#ifdef NOISY_SIGALRM
			log_error_time();
			fprintf(stderr, "mime_hashtable[%d] has %d entries\n",
					i, count);
#endif
			total += count;
		}
	}
	log_error_time();
	fprintf(stderr, "mime_hashtable has %d total entries\n",
			total);

	total = 0;
	for (i = 0; i < PASSWD_HASHTABLE_SIZE; ++i) { /* these limits OK? */
		if (passwd_hashtable[i]) {
			temp = passwd_hashtable[i];
			count = 0;
			while (temp) {
				temp = temp->next;
				++count;
			}
#ifdef NOISY_SIGALRM
			log_error_time();
			fprintf(stderr, "passwd_hashtable[%d] has %d entries\n",
					i, count);
#endif
			total += count;
		}
	}

	log_error_time();
	fprintf(stderr, "passwd_hashtable has %d total entries\n",
			total);

}
