/**
 * Copyright (c) 2008, SnakeYAML
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.yaml.snakeyaml.issues.issue525;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.Test;
import org.yaml.snakeyaml.Util;
import org.yaml.snakeyaml.Yaml;
import org.yaml.snakeyaml.error.YAMLException;

public class FuzzyStackOverflowTest {

  @Test
  public void parseOpenUnmatchedMappings() {
    try {
      Yaml yaml = new Yaml();
      String strYaml = Util.getLocalResource("fuzzer/YamlFuzzer-4626423186325504");
      yaml.load(strYaml);
      fail("Should report invalid YAML");
    } catch (YAMLException e) {
      assertTrue(false);
    } catch (Error e) {
      //TODO FIXME stackoverflow
    }
  }
}
