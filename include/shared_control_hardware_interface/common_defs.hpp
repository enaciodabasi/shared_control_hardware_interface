/**
 * @file common_defs.hpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef COMMON_DEFS_HPP_
#define COMMON_DEFS_HPP_

namespace schi
{

  enum ReturnCode
  {
    Error = 0,
    Success = 1,
    SharedSegmentCreateError,
    SharedSegmentNamedObjectConstructError,
    SharedSegmentOpenError,
    SharedSegmentNamedObjectSearchError,
  };

} // End of namespace schi


#endif // COMMON_DEFS_HPP_