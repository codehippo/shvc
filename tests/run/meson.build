bats = find_program('bats')

bats_tests = [
  'foo',
]

foreach bats_test : bats_tests
  test(
    'runtest-' + bats_test,
    bash,
    args: [
      test_driver,
      '-n',
      bats.full_path(),
      '--tap',
      join_paths(meson.current_source_dir(), bats_test + '.bats'),
    ],
    env: ['TEST_FOO=' + foo.full_path()],
    protocol: 'tap',
    depends: foo,
  )
endforeach
