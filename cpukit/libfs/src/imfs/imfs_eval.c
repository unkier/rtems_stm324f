/*
 *  Evaluation IMFS Node Support Routines
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

#define RTEMS_LIBIO_PERMS_RX (RTEMS_LIBIO_PERMS_SEARCH | RTEMS_LIBIO_PERMS_READ)
#define RTEMS_LIBIO_PERMS_WX (RTEMS_LIBIO_PERMS_SEARCH | RTEMS_LIBIO_PERMS_WRITE)

#define MAXSYMLINK 5

int IMFS_Set_handlers(
  rtems_filesystem_location_info_t   *loc
)
{
  IMFS_jnode_t    *node = loc->node_access;
  IMFS_fs_info_t  *fs_info;

  fs_info = loc->mt_entry->fs_info;
  switch( node->type ) {
    case IMFS_DIRECTORY:
      loc->handlers = fs_info->directory_handlers;
      break;
    case IMFS_DEVICE:
      loc->handlers = &IMFS_device_handlers;
      break;
    case IMFS_SYM_LINK:
    case IMFS_HARD_LINK:
      loc->handlers = &IMFS_link_handlers;
      break;
    case IMFS_LINEAR_FILE:
      loc->handlers = fs_info->memfile_handlers;
      break;
    case IMFS_MEMORY_FILE:
      loc->handlers = fs_info->memfile_handlers;
      break;
    case IMFS_FIFO:
      loc->handlers = fs_info->fifo_handlers;
      break;
  }

  return 0;
}

/*
 *  IMFS_evaluate_permission
 *
 *  The following routine evaluates that we have permission
 *  to do flags on the node.
 */
static int IMFS_evaluate_permission(
  rtems_filesystem_location_info_t  *node,
  int                                flags
)
{
  uid_t         st_uid;
  gid_t         st_gid;
  IMFS_jnode_t *jnode;
  int           flags_to_test;

  if ( !rtems_libio_is_valid_perms( flags ) )
    rtems_set_errno_and_return_minus_one( EPERM );

  jnode = node->node_access;

#if defined(RTEMS_POSIX_API)
  st_uid = geteuid();
  st_gid = getegid();
#else
  st_uid = jnode->st_uid;
  st_gid = jnode->st_gid;
#endif

  /*
   * Check if I am owner or a group member or someone else.
   */

  flags_to_test = flags;

  if ( st_uid == jnode->st_uid )
    flags_to_test <<= 6;
  else if ( st_gid == jnode->st_gid )
    flags_to_test <<= 3;
  else {
    /* must be other - do nothing */;
  }

  /*
   * If all of the flags are set we have permission
   * to do this.
   */
  if ( ( flags_to_test & jnode->st_mode) == flags_to_test )
    return 1;

  return 0;
}

/*
 *  IMFS_evaluate_hard_link
 *
 *  The following routine evaluates a hardlink to the actual node.
 */

static int IMFS_evaluate_hard_link(
  rtems_filesystem_location_info_t  *node,   /* IN/OUT */
  int                                flags   /* IN     */
)
{
  IMFS_jnode_t                     *jnode  = node->node_access;
  int                               result = 0;

  /*
   * Check for things that should never happen.
   */
  IMFS_assert( jnode->type == IMFS_HARD_LINK );

  /*
   * Set the hard link value and the handlers.
   */
  node->node_access = jnode->info.hard_link.link_node;

  IMFS_Set_handlers( node );

  /*
   * Verify we have the correct permissions for this node.
   */

  if ( !IMFS_evaluate_permission( node, flags ) )
    rtems_set_errno_and_return_minus_one( EACCES );

  return result;
}


/*
 *  IMFS_evaluate_sym_link
 *
 *  The following routine evaluates a symbolic link to the actual node.
 */

static int IMFS_evaluate_sym_link(
  rtems_filesystem_location_info_t  *node,   /* IN/OUT */
  int                                flags   /* IN     */
)
{
  IMFS_jnode_t                     *jnode  = node->node_access;
  int                               result = 0;
  int                               i;

  /*
   * Check for things that should never happen.
   */
  IMFS_assert( jnode->type == IMFS_SYM_LINK );
  IMFS_assert( jnode->Parent );

  /*
   * Move the node_access to either the symbolic links parent or
   * root depending on the symbolic links path.
   */
  node->node_access = jnode->Parent;

  rtems_filesystem_get_sym_start_loc(
    jnode->info.sym_link.name,
    &i,
    node
  );

  /*
   * Use eval path to evaluate the path of the symbolic link.
   */
  result = IMFS_eval_path(
    &jnode->info.sym_link.name[i],
    strlen( &jnode->info.sym_link.name[i] ),
    flags,
    node
  );

  IMFS_Set_handlers( node );

  /*
   * Verify we have the correct permissions for this node.
   */
  if ( !IMFS_evaluate_permission( node, flags ) )
    rtems_set_errno_and_return_minus_one( EACCES );

  return result;
}

/*
 *  IMFS_evaluate_link
 *
 *  The following routine returns the real node pointed to by a link.
 */
int IMFS_evaluate_link(
  rtems_filesystem_location_info_t  *node,   /* IN/OUT */
  int                                flags   /* IN     */
)
{
  IMFS_jnode_t                     *jnode;
  int                               result = 0;

  do {
    jnode  = node->node_access;

    /*
     * Increment and check the link counter.
     */

    rtems_filesystem_link_counts ++;
    if ( rtems_filesystem_link_counts > MAXSYMLINK ) {
      rtems_filesystem_link_counts = 0;
      rtems_set_errno_and_return_minus_one( ELOOP );
    }

    /*
     *  Follow the Link node.
     */

    if ( jnode->type == IMFS_HARD_LINK )
      result = IMFS_evaluate_hard_link( node, flags );

    else if (jnode->type == IMFS_SYM_LINK )
      result = IMFS_evaluate_sym_link( node, flags );

  } while ( ( result == 0 ) && ( ( jnode->type == IMFS_SYM_LINK  ) ||
                                 ( jnode->type == IMFS_HARD_LINK ) ) );

  /*
   * Clear link counter.
   */

  rtems_filesystem_link_counts = 0;

  return result;
}

/*
 * IMFS_skip_separator
 *
 * Skip the separator in the path.
 */
static void IMFS_skip_separator (
   const char *path,       /* IN     */
   size_t     *len,        /* IN/OUT */
   int        *index       /* IN/OUT */
)
{
  while ( IMFS_is_separator( path[*index] ) && path[*index] && *len ) {
    ++(*index);
    --(*len);
  }
}

/*
 *  IMFS_evaluate_for_make
 *
 *  The following routine evaluate path for a new node to be created.
 *  pathloc is returned with a pointer to the parent of the new node.
 *  name is returned with a pointer to the first character in the
 *  new node name.  The parent node is verified to be a directory.
 */

int IMFS_evaluate_for_make(
  const char                         *path,       /* IN     */
  rtems_filesystem_location_info_t   *pathloc,    /* IN/OUT */
  const char                        **name        /* OUT    */
                           )
{
  int               i = 0;
  int               len;
  IMFS_token_types  type;
  char              token[ IMFS_NAME_MAX + 1 ];
  IMFS_jnode_t     *node;
  bool              done = false;
  size_t            pathlen;
  int               result;

  /*
   * This was filled in by the caller and is valid in the
   * mount table.
   */
  node = pathloc->node_access;

  /*
   * Get the path length.
   */
  pathlen = strlen( path );
  /*
   *  Evaluate all tokens until we are done or an error occurs.
   */

  while( !done ) {

    type = IMFS_get_token( &path[i], pathlen, token, &len );
    pathlen -= len;
    i +=  len;

    if ( !pathloc->node_access )
      rtems_set_errno_and_return_minus_one( ENOENT );

    /*
     * I cannot move out of this directory without execute permission.
     */

    if ( type != IMFS_NO_MORE_PATH )
      if ( node->type == IMFS_DIRECTORY )
        if ( !IMFS_evaluate_permission( pathloc, RTEMS_LIBIO_PERMS_SEARCH ) )
          rtems_set_errno_and_return_minus_one( EACCES );

    node = pathloc->node_access;

    switch( type ) {

      case IMFS_UP_DIR:
        /*
         *  Am I at the root of all filesystems? (chroot'ed?)
         */

        if ( pathloc->node_access == rtems_filesystem_root.node_access )
          break;       /* Throw out the .. in this case */


        /*
         * Am I at the root of this mounted filesystem?
         */

        if ( rtems_filesystem_is_root_location( pathloc ) ) {

          /*
           *  Am I at the root of all filesystems?
           */

          if ( pathloc->node_access == rtems_filesystem_root.node_access ) {
            break;

          } else {
            *pathloc = pathloc->mt_entry->mt_point_node;
            return (*pathloc->ops->evalformake_h)( &path[i-len], pathloc, name );
          }
        } else {

          if ( !node->Parent )
            rtems_set_errno_and_return_minus_one( ENOENT );

          node = node->Parent;
        }

        pathloc->node_access = node;
        break;

      case IMFS_NAME:

        if ( node->type == IMFS_HARD_LINK ) {

          result = IMFS_evaluate_link( pathloc, 0 );
          if ( result == -1 )
            return -1;

        } else if ( node->type == IMFS_SYM_LINK ) {

          result = IMFS_evaluate_link( pathloc, 0 );

          if ( result == -1 )
            return -1;
        }

        node = pathloc->node_access;
        if ( !node )
          rtems_set_errno_and_return_minus_one( ENOTDIR );

        /*
         * Only a directory can be decended into.
         */

        if ( node->type != IMFS_DIRECTORY )
          rtems_set_errno_and_return_minus_one( ENOTDIR );

        /*
         * Find the token name in the present location.
         */

        node = IMFS_find_match_in_dir( node, token );

        /*
         * If there is no node we have found the name of the node we
         * wish to create.
         */

        if ( ! node )
          done = true;
        else {
        if (( node->type == IMFS_DIRECTORY ) && ( node->info.directory.mt_fs != NULL )) {
            IMFS_skip_separator( path, &pathlen, &i);
            if ((path[i] != '.') || (path[i + 1] != '.')) {
              *pathloc = node->info.directory.mt_fs->mt_fs_root;
              return (*pathloc->ops->evalformake_h)( &path[i],
                                                     pathloc,
                                                     name );
            }
            i += 2;
            pathlen -= 2;
            node = node->Parent;
          }
          pathloc->node_access = node;
        }
        break;

      case IMFS_NO_MORE_PATH:
        rtems_set_errno_and_return_minus_one( EEXIST );
        break;

      case IMFS_INVALID_TOKEN:
        rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
        break;

      case IMFS_CURRENT_DIR:
        break;
    }
  }

  *name = &path[ i - len ];

  /*
   * We have evaluated the path as far as we can.
   * Verify there is not any invalid stuff at the end of the name.
   */

  for( ; path[i] != '\0'; i++) {
    if ( !IMFS_is_separator( path[ i ] ) )
      rtems_set_errno_and_return_minus_one( ENOENT );
  }

  /*
   * Verify we can execute and write to this directory.
   */

  result = IMFS_Set_handlers( pathloc );

  /*
   * The returned node must be a directory
   */
  node = pathloc->node_access;
  if ( node->type != IMFS_DIRECTORY )
    rtems_set_errno_and_return_minus_one( ENOTDIR );

  /*
   * We must have Write and execute permission on the returned node.
   */

  if ( !IMFS_evaluate_permission( pathloc, RTEMS_LIBIO_PERMS_WX ) )
    rtems_set_errno_and_return_minus_one( EACCES );

  return result;
}

/*
 *  IMFS_eval_path
 *
 *  The following routine evaluate path for a node that wishes to be
 *  accessed with mode.  pathloc is returned with a pointer to the
 *  node to be accessed.
 */

int IMFS_eval_path(
  const char                        *pathname,     /* IN     */
  size_t                             pathnamelen,  /* IN     */
  int                                flags,        /* IN     */
  rtems_filesystem_location_info_t  *pathloc       /* IN/OUT */
                   )
{
  int               i = 0;
  int               len;
  IMFS_token_types  type = IMFS_CURRENT_DIR;
  char              token[ IMFS_NAME_MAX + 1 ];
  IMFS_jnode_t     *node;
  int               result;

  if ( !rtems_libio_is_valid_perms( flags ) ) {
    rtems_set_errno_and_return_minus_one( EIO );
  }

  /*
   *  This was filled in by the caller and is valid in the
   *  mount table.
   */

  node = pathloc->node_access;

  /*
   *  Evaluate all tokens until we are done or an error occurs.
   */

  while( (type != IMFS_NO_MORE_PATH) && (type != IMFS_INVALID_TOKEN) ) {

    type = IMFS_get_token( &pathname[i], pathnamelen, token, &len );
    pathnamelen -= len;
    i += len;

    if ( !pathloc->node_access )
      rtems_set_errno_and_return_minus_one( ENOENT );

    /*
     * I cannot move out of this directory without execute permission.
     */
    if ( type != IMFS_NO_MORE_PATH )
      if ( node->type == IMFS_DIRECTORY )
        if ( !IMFS_evaluate_permission( pathloc, RTEMS_LIBIO_PERMS_SEARCH ) )
          rtems_set_errno_and_return_minus_one( EACCES );

    node = pathloc->node_access;

    switch( type ) {
      case IMFS_UP_DIR:
        /*
         *  Am I at the root of all filesystems? (chroot'ed?)
         */

        if ( pathloc->node_access == rtems_filesystem_root.node_access )
          break;       /* Throw out the .. in this case */

        /*
         *  Am I at the root of this mounted filesystem?
         */

        if ( rtems_filesystem_is_root_location( pathloc ) ) {

          /*
           *  Am I at the root of all filesystems?
           */

          if ( pathloc->node_access == rtems_filesystem_root.node_access ) {
            break;       /* Throw out the .. in this case */
          } else {
            *pathloc = pathloc->mt_entry->mt_point_node;
            return (*pathloc->ops->evalpath_h)(&(pathname[i-len]),
                                               pathnamelen+len,
                                               flags,pathloc);
          }
        } else {

          if ( !node->Parent )
            rtems_set_errno_and_return_minus_one( ENOENT );

          node = node->Parent;
          pathloc->node_access = node;

        }

        pathloc->node_access = node;
        break;

      case IMFS_NAME:
        /*
         *  If we are at a link follow it.
         */
        if ( node->type == IMFS_HARD_LINK ) {
          IMFS_evaluate_hard_link( pathloc, 0 );
          node = pathloc->node_access;

          /*
           * It would be a design error if we evaluated the link and
           * was broken.
           */
          IMFS_assert( node );

        } else if ( node->type == IMFS_SYM_LINK ) {
          result = IMFS_evaluate_sym_link( pathloc, 0 );

          /*
           *  In contrast to a hard link, it is possible to have a broken
           *  symbolic link.
           */
          node = pathloc->node_access;
          if ( result == -1 )
            return -1;
        }

        /*
         *  Only a directory can be decended into.
         */
        if ( node->type != IMFS_DIRECTORY )
          rtems_set_errno_and_return_minus_one( ENOTDIR );

        /*
         *  Find the token name in the current node.
         */
        node = IMFS_find_match_in_dir( node, token );
        if ( !node )
          rtems_set_errno_and_return_minus_one( ENOENT );

        /*
         *  If we are at a node that is a mount point so current directory
         *  actually exists on the mounted file system and not in the node that
         *  contains the mount point node. For example a stat of the mount
         *  point should return the details of the root of the mounted file
         *  system not the mount point node of parent file system.
         *
         *  If the node we have just moved to is a mount point do not loop and
         *  get the token because the token may be suitable for the mounted
         *  file system and not the IMFS. For example the IMFS length is
         *  limited. If the token is a parent directory move back up otherwise
         *  set loc to the new fs root node and let them finish evaluating the
         *  path.
         */
        if (( node->type == IMFS_DIRECTORY ) && ( node->info.directory.mt_fs != NULL )) {
          IMFS_skip_separator( pathname, &pathnamelen, &i);
          if ((pathname[i] != '.') || (pathname[i + 1] != '.')) {
            *pathloc = node->info.directory.mt_fs->mt_fs_root;
            return (*pathloc->ops->evalpath_h)( &pathname[i],
                                                pathnamelen,
                                                flags, pathloc );
          }
          i += 2;
          pathnamelen -= 2;
          node = node->Parent;
        }

        /*
         *  Set the node access to the point we have found.
         */
        pathloc->node_access = node;
        break;

      case IMFS_NO_MORE_PATH:
      case IMFS_CURRENT_DIR:
        break;

      case IMFS_INVALID_TOKEN:
        rtems_set_errno_and_return_minus_one( ENAMETOOLONG );
        break;

    }
  }

  /*
   *  Always return the root node.
   *
   *  If we are at a node that is a mount point. Set loc to the
   *  new fs root node and let let the mounted filesystem set the handlers.
   *
   *  NOTE: The behavior of stat() on a mount point appears to be questionable.
   */

  if ( node->type == IMFS_DIRECTORY ) {
    if ( node->info.directory.mt_fs != NULL ) {
      *pathloc = node->info.directory.mt_fs->mt_fs_root;
      return (*pathloc->ops->evalpath_h)( &pathname[i-len],
                                          pathnamelen+len,
                                          flags, pathloc );
    } else {
      result = IMFS_Set_handlers( pathloc );
    }
  } else {
    result = IMFS_Set_handlers( pathloc );
  }

  /*
   * Verify we have the correct permissions for this node.
   */

  if ( !IMFS_evaluate_permission( pathloc, flags ) )
    rtems_set_errno_and_return_minus_one( EACCES );

  return result;
}
