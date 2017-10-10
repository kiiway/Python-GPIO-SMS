# cmake <http://www.cmake.org> script to setup dummy phone for testing
# Copyright (c) 2007-2009 Michal Cihar
# vim: expandtab sw=4 ts=4 sts=4 ft=cmake:

if (WITH_BACKUP)
file(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/1")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/2")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/3")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/4")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/5")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/pbk/ME")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/pbk/SM")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/calendar")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/todo")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/note")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir1/dir1-2/dir1-2-3")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/dir2-4")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/dir2-5")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/testdir")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/dir2-5/file1" "This is testing file1!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/dir2-5/file2" "This is testing file2!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/dir2-5/file3" "This is testing file3!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/dir2-5/file4" "This is testing file4!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/file5" "This is testing file5!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/fs/dir2/file5" "This is testing file6!")
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/01.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/1/1" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/90.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/2/10" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/99.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/3/42" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/80.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/4/15" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/smsbackups/mms-part.smsbackup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/4/20" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/smsbackups/mms-part2.smsbackup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/sms/4/21" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/private.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/pbk/ME/1" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/yahoo.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/pbk/ME/101" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/photo.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/pbk/ME/103" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/many-lines.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/pbk/SM/1" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/rrule-1.0.vcs" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/calendar/2" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/dtstart.ics" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/calendar/22" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/02.vcs" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/todo/3" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/01.ics" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy/todo/22" COPYONLY)
file(REMOVE_RECURSE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/1")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/2")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/3")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/4")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/5")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/pbk/ME")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/pbk/SM")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/calendar")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/todo")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/note")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir1/dir1-2/dir1-2-3")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/dir2-4")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/dir2-5")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/testdir")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/dir2-5/file1" "This is testing file1!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/dir2-5/file2" "This is testing file2!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/dir2-5/file3" "This is testing file3!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/dir2-5/file4" "This is testing file4!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/file5" "This is testing file5!")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/fs/dir2/file5" "This is testing file6!")
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/01.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/1/1" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/90.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/2/10" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/99.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/3/42" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/at-sms-encode/80.backup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/4/15" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/smsbackups/mms-part.smsbackup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/4/20" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/smsbackups/mms-part2.smsbackup" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/sms/4/21" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/private.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/pbk/ME/1" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/yahoo.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/pbk/ME/101" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/photo.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/pbk/ME/103" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcards/many-lines.vcf" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/pbk/SM/1" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/rrule-1.0.vcs" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/calendar/2" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/dtstart.ics" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/calendar/22" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/02.vcs" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/todo/3" COPYONLY)
configure_file("${Gammu_SOURCE_DIR}/tests/vcal/01.ics" "${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext/todo/22" COPYONLY)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/.gammurc" "
# Generated gammurc for test purposes
[gammu]
model = dummy
connection = none
port = ${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy
gammuloc = /dev/null
logformat = textall
logfile = ${CMAKE_CURRENT_BINARY_DIR}/gammu.log

[gammu2]
model = dummy
features = DISABLE_GETNEXT
connection = none
port = ${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy-nonext
gammuloc = /dev/null
logformat = textall
logfile = ${CMAKE_CURRENT_BINARY_DIR}/gammu.log

[gammu99]
model = dummy
connection = none
port = ${CMAKE_CURRENT_BINARY_DIR}/.gammu-dummy
gammuloc = /dev/null
")
endif (WITH_BACKUP)
