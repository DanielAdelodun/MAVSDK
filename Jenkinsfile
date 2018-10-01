#!/usr/bin/env groovy

def user_id
def group_id
node {
  user_id = sh(returnStdout: true, script: 'id -u').trim()
  group_id = sh(returnStdout: true, script: 'id -g').trim()
}

pipeline {
  agent none

  stages {
    stage('Build') {
      parallel {

        stage('Ubuntu 16.04 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
              args '-e LOCAL_USER_ID=user_id'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
            sh 'sudo make default install'
            sh 'mkdir -p example/takeoff_land/build'
            sh 'cd example/takeoff_land/build && cmake .. && make'
          }
        }

        stage('Ubuntu 16.04 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

        stage('Ubuntu 18.04 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-18.04:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

        stage('Ubuntu 18.04 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-18.04:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

        stage('Fedora 27 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-27:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

        stage('Fedora 27 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-27:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

        stage('Fedora 28 Debug') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-28:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Debug BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

        stage('Fedora 28 Release') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-fedora-28:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'make BUILD_TYPE=Release BUILD_BACKEND=1'
            sh 'build/default/unit_tests_runner'
            sh 'build/default/backend/test/unit_tests_backend'
          }
        }

      } // parallel
    } // Build

    stage('Test') {
      parallel {

        stage('check style') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
            }
          }
          steps {
            sh 'make fix_style'
          }
        }

        stage('example/takeoff_land') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'mkdir -p example/takeoff_land/build'
            sh 'cd example/takeoff_land/build && cmake ..'
            // FIXME sh 'make -C example/takeoff_land/build'
          }
        }

        stage('example/fly_mission') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'mkdir -p example/fly_mission/build'
            sh 'cd example/fly_mission/build && cmake ..'
            // FIXME sh 'make -C example/fly_mission/build'
          }
        }

        stage('example/offboard_velocity') {
          agent {
            docker {
              image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
            }
          }
          steps {
            sh 'git submodule deinit -f .'
            sh 'git clean -ff -x -d .'
            sh 'git submodule update --init --recursive --force'
            sh 'mkdir -p example/offboard_velocity/build'
            sh 'cd example/offboard_velocity/build && cmake ..'
            // FIXME sh 'make -C example/offboard_velocity/build'
          }
        }

      } // parallel
    } // stage Test

    stage('Generate Docs') {
      agent {
        docker {
          image 'dronecode/dronecode-sdk-ubuntu-16.04:2018-10-01'
        }
      }
      steps {
        sh 'git submodule deinit -f .'
        sh 'git clean -ff -x -d .'
        sh 'git submodule update --init --recursive --force'
        sh './generate_docs.sh'
      }
    } // stage Generate Docs

  } // stages

  options {
    buildDiscarder(logRotator(numToKeepStr: '10', artifactDaysToKeepStr: '30'))
    timeout(time: 60, unit: 'MINUTES')
  }
}
