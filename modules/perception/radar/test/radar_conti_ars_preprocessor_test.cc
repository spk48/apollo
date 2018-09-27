// Copyright 2018 Baidu Inc. All Rights Reserved.
// @author: Chongchong Li (lichongchong@baidu.com)
// @file: radar_conti_ars_preprocessor_test.cc
// @brief: unit test for conti_ars_preprocessor

#include <gtest/gtest.h>
#include "modules/perception/radar/lib/preprocessor/conti_ars_preprocessor/conti_ars_preprocessor.h"

namespace apollo {
namespace perception {
namespace lib {

DECLARE_string(work_root);

}  // namespace lib
}  // namespace perception
}  // namespace apollo

namespace apollo {
namespace perception {
namespace radar {

class ContiArsPreprocessorTest : public testing::Test {
 public:
  void SetMaxRadarIdx() {
    preprocessor.current_idx_ = MAX_RADAR_IDX - 1;
  }
 protected:
  ContiArsPreprocessor preprocessor;
};

TEST_F(ContiArsPreprocessorTest, init) {
  float delay_time = preprocessor.GetDelayTime();
  EXPECT_FLOAT_EQ(delay_time, 0.0);
  lib::FLAGS_work_root = "./radar_test_data/preprocessor";
  bool init_result = preprocessor.Init();
  EXPECT_TRUE(init_result);
  delay_time = preprocessor.GetDelayTime();
  EXPECT_FLOAT_EQ(delay_time, 0.07);
  EXPECT_EQ(preprocessor.Name(), "ContiArsPreprocessor");
}

TEST_F(ContiArsPreprocessorTest, preprocess) {
  ContiRadar raw_obs;
  PreprocessorOptions options;
  ContiRadar corrected_obs;

  ContiRadarObs* conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(80);
  conti_obs->set_meas_state(2);
  Header obj_header;
  obj_header.set_timestamp_sec(151237772.305345434);
  obj_header.set_radar_timestamp(151237772305345434);
  obj_header.set_module_name("radar");
  obj_header.set_sequence_num(0);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(0);
  conti_obs->set_meas_state(2);
  obj_header.set_timestamp_sec(151237772.375345434);
  obj_header.set_radar_timestamp(151237772375345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(1);
  conti_obs->set_meas_state(0);
  obj_header.set_timestamp_sec(151237772.385345434);
  obj_header.set_radar_timestamp(151237772385345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(2);
  conti_obs->set_meas_state(3);
  obj_header.set_timestamp_sec(151237772.385345434);
  obj_header.set_radar_timestamp(151237772385345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(0);
  conti_obs->set_meas_state(2);
  obj_header.set_timestamp_sec(151237772.585345434);
  obj_header.set_radar_timestamp(151237772385345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  preprocessor.Preprocess(raw_obs, options, &corrected_obs);

  EXPECT_EQ(corrected_obs.contiobs_size(), 3);
  EXPECT_EQ(corrected_obs.contiobs(0).obstacle_id(), 1);
  EXPECT_EQ(corrected_obs.contiobs(1).obstacle_id(), 2);
  EXPECT_EQ(corrected_obs.contiobs(2).obstacle_id(), 3);

  raw_obs.clear_contiobs();
  corrected_obs.clear_contiobs();

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(1);
  conti_obs->set_meas_state(1);
  obj_header.set_timestamp_sec(151237772.445345434);
  obj_header.set_radar_timestamp(151237772445345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(2);
  conti_obs->set_meas_state(2);
  obj_header.set_timestamp_sec(151237772.455345434);
  obj_header.set_radar_timestamp(151237772455345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(3);
  conti_obs->set_meas_state(1);
  obj_header.set_timestamp_sec(151237772.455345434);
  obj_header.set_radar_timestamp(151237772455345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  preprocessor.Preprocess(raw_obs, options, &corrected_obs);

  EXPECT_EQ(corrected_obs.contiobs_size(), 3);
  EXPECT_EQ(corrected_obs.contiobs(0).obstacle_id(), 4);
  EXPECT_EQ(corrected_obs.contiobs(1).obstacle_id(), 3);
  EXPECT_EQ(corrected_obs.contiobs(2).obstacle_id(), 5);

  raw_obs.clear_contiobs();
  corrected_obs.clear_contiobs();

  SetMaxRadarIdx();
  conti_obs = raw_obs.add_contiobs();
  conti_obs->set_obstacle_id(50);
  conti_obs->set_meas_state(1);
  obj_header.set_timestamp_sec(151237772.525345434);
  obj_header.set_radar_timestamp(151237772525345434);
  conti_obs->mutable_header()->CopyFrom(obj_header);

  preprocessor.Preprocess(raw_obs, options, &corrected_obs);
  EXPECT_EQ(corrected_obs.contiobs_size(), 1);
  EXPECT_EQ(corrected_obs.contiobs(0).obstacle_id(), 1);
}

}  // namespace radar
}  // namespace perception
}  // namespace apollo
