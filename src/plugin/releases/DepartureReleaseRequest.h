#pragma once

namespace UKControllerPlugin::Controller {
    class ControllerPosition;
} // namespace UKControllerPlugin::Controller

namespace UKControllerPlugin::Releases {

    class DepartureReleaseRequest
    {
        public:
        DepartureReleaseRequest(
            int id,
            std::string callsign,
            const std::shared_ptr<Controller::ControllerPosition>& requestingController,
            const std::shared_ptr<Controller::ControllerPosition>& targetController,
            std::chrono::system_clock::time_point requestExpiresAt);

        void Acknowledge();
        void Reject(std::string remarks);
        void Approve(
            std::chrono::system_clock::time_point releasedAtTime,
            std::chrono::system_clock::time_point releaseExpiresAt,
            std::string remarks);

        void Approve(std::chrono::system_clock::time_point releasedAtTime, std::string remarks);

        [[nodiscard]] auto Id() const -> int;
        [[nodiscard]] auto RequiresDecision() const -> bool;
        [[nodiscard]] auto Callsign() const -> const std::string&;
        [[nodiscard]] auto RequestingController() const -> std::shared_ptr<Controller::ControllerPosition>;
        [[nodiscard]] auto RequestingControllerId() const -> int;
        [[nodiscard]] auto TargetController() const -> std::shared_ptr<Controller::ControllerPosition>;
        [[nodiscard]] auto TargetControllerId() const -> int;
        [[nodiscard]] auto Acknowledged() const -> bool;
        [[nodiscard]] auto Rejected() const -> bool;
        [[nodiscard]] auto Approved() const -> bool;
        [[nodiscard]] auto RequestExpired() const -> bool;
        [[nodiscard]] auto ApprovalExpired() const -> bool;
        [[nodiscard]] auto AwaitingReleasedTime() const -> bool;
        [[nodiscard]] auto ApprovedWithNoExpiry() const -> bool;
        [[nodiscard]] auto Remarks() const -> const std::string&;
        [[nodiscard]] auto RequestExpiryTime() const -> const std::chrono::system_clock::time_point&;
        [[nodiscard]] auto ReleaseExpiryTime() const -> const std::chrono::system_clock::time_point&;
        [[nodiscard]] auto ReleasedAtTime() const -> const std::chrono::system_clock::time_point&;
        [[nodiscard]] auto RejectedAtTime() const -> const std::chrono::system_clock::time_point&;
        [[nodiscard]] auto AcknowledgedAtTime() const -> const std::chrono::system_clock::time_point&;
        [[nodiscard]] auto CreatedAt() const -> const std::chrono::system_clock::time_point&;

        private:
        static inline const std::chrono::system_clock::time_point noTime =
            (std::chrono::system_clock::time_point::min)();

        static inline const std::chrono::system_clock::time_point noTimeMax =
            (std::chrono::system_clock::time_point::max)() - std::chrono::hours(500);

        // The id of the request
        int id;

        // The callsign for the request
        std::string callsign;

        // Who's requesting the release
        std::shared_ptr<Controller::ControllerPosition> requestingController;

        // Who's the target of the release request
        std::shared_ptr<Controller::ControllerPosition> targetController;

        // When the the request expires
        std::chrono::system_clock::time_point requestExpiresAt;

        // What time the release was acknowledged
        std::chrono::system_clock::time_point acknowledgedAtTime = noTime;

        // What time the release was rejected
        std::chrono::system_clock::time_point rejectedAtTime = noTime;

        // The time the aircraft has been approved for release
        std::chrono::system_clock::time_point releasedAtTime = noTime;

        // The time that the release expires
        std::chrono::system_clock::time_point releaseExpiresAt = noTimeMax;

        // When the request was created
        std::chrono::system_clock::time_point createdAt;

        // Remarks relating to the release being approved or rejected
        std::string remarks;
    };
} // namespace UKControllerPlugin::Releases
