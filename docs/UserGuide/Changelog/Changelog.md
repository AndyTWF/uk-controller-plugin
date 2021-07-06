# UK Controller Plugin Changelog

## [3.1.4](https://github.com/VATSIM-UK/uk-controller-plugin/compare/3.1.3...3.1.4) (2021-07-05)


### Bug Fixes

* **departure-releases:** add acknowledgement colour for departure release status indicator ([#296](https://github.com/VATSIM-UK/uk-controller-plugin/issues/296)) ([1afcc53](https://github.com/VATSIM-UK/uk-controller-plugin/commit/1afcc5356b0ade6af422d5a3b7149cde7706473c)), closes [#295](https://github.com/VATSIM-UK/uk-controller-plugin/issues/295)
* **departure-releases:** treat releases at current minute as released at exactly now ([#298](https://github.com/VATSIM-UK/uk-controller-plugin/issues/298)) ([3e1dd6e](https://github.com/VATSIM-UK/uk-controller-plugin/commit/3e1dd6e9b6fa1976514811200d2ea3a0c5dc10f3)), closes [#297](https://github.com/VATSIM-UK/uk-controller-plugin/issues/297)

## [3.1.3](https://github.com/VATSIM-UK/uk-controller-plugin/compare/3.1.2...3.1.3) (2021-07-03)


### Bug Fixes

* **departure-releases:** fix colours in the release status view and add documentation ([#293](https://github.com/VATSIM-UK/uk-controller-plugin/issues/293)) ([f6ba6a1](https://github.com/VATSIM-UK/uk-controller-plugin/commit/f6ba6a10202938ca9fc974df7df877660c3ffa9c)), closes [#290](https://github.com/VATSIM-UK/uk-controller-plugin/issues/290)
* **departure-releases:** fix crash when acknowledging departure releases ([#292](https://github.com/VATSIM-UK/uk-controller-plugin/issues/292)) ([41345f2](https://github.com/VATSIM-UK/uk-controller-plugin/commit/41345f257b1fa5922aa9a0d349fdbae9c5de98df)), closes [#289](https://github.com/VATSIM-UK/uk-controller-plugin/issues/289)

## [3.1.2](https://github.com/VATSIM-UK/uk-controller-plugin/compare/3.1.1...3.1.2) (2021-07-03)


### Bug Fixes

* **departure-releases:** Fix releases not being properly approved through decision list ([#290](https://github.com/VATSIM-UK/uk-controller-plugin/issues/290)) ([b8fbdb5](https://github.com/VATSIM-UK/uk-controller-plugin/commit/b8fbdb5776d40bb3e235a3e5430a5fbbb396165c))

## [3.1.1](https://github.com/VATSIM-UK/uk-controller-plugin/compare/3.1.0...3.1.1) (2021-07-03)


### Bug Fixes

* **squawks:** assign squawks with C-Mode correlation when aircraft connect ([#287](https://github.com/VATSIM-UK/uk-controller-plugin/issues/287)) ([464de37](https://github.com/VATSIM-UK/uk-controller-plugin/commit/464de37f4cecea93c8e0fda9eec3f87985fe7c86)), closes [#284](https://github.com/VATSIM-UK/uk-controller-plugin/issues/284) [#285](https://github.com/VATSIM-UK/uk-controller-plugin/issues/285) [#286](https://github.com/VATSIM-UK/uk-controller-plugin/issues/286)

# [3.1.0](https://github.com/VATSIM-UK/uk-controller-plugin/compare/3.0.1...3.1.0) (2021-07-02)


### Bug Fixes

* **semantic-release:** add missing semantic release plugins ([d9f7851](https://github.com/VATSIM-UK/uk-controller-plugin/commit/d9f785110240a0e402055359039fb5570fa802f1))


### Features

* **releases:** Add electronic departure releases ([#272](https://github.com/VATSIM-UK/uk-controller-plugin/issues/272)) ([9754ad5](https://github.com/VATSIM-UK/uk-controller-plugin/commit/9754ad540e39de0164bd1de13a7628287e16be56)), closes [#267](https://github.com/VATSIM-UK/uk-controller-plugin/issues/267) [#268](https://github.com/VATSIM-UK/uk-controller-plugin/issues/268) [#269](https://github.com/VATSIM-UK/uk-controller-plugin/issues/269) [#271](https://github.com/VATSIM-UK/uk-controller-plugin/issues/271) [#281](https://github.com/VATSIM-UK/uk-controller-plugin/issues/281)

## Version 3.0.1

- Fix a bug where initial altitudes updates are repeatedly triggered on plugins running on a proxy EuroScope connection.

## Version 3.0.0

- The UK Controller Plugin is now self-updating.

## Versions before 3.0.0

For changes in versions prior to 3.0.0, please see the download section of the VATSIM UK Website.
