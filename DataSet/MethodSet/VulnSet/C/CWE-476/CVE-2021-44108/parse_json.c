static int parse_json(ogs_sbi_message_t *message,
        char *content_type, char *json)
{
    int rv = OGS_OK;
    cJSON *item = NULL;

    ogs_assert(message);

    if (!json)
        return OGS_OK;

    if (!content_type) {
        ogs_error("No Content-type");
        return OGS_ERROR;
    }

    ogs_log_print(OGS_LOG_TRACE, "%s", json);
    item = cJSON_Parse(json);
    if (!item) {
        ogs_error("JSON parse error");
        return OGS_ERROR;
    }

    if (content_type &&
        !strncmp(content_type, OGS_SBI_CONTENT_PROBLEM_TYPE,
            strlen(OGS_SBI_CONTENT_PROBLEM_TYPE))) {
        message->ProblemDetails = OpenAPI_problem_details_parseFromJSON(item);
    } else if (content_type &&
                !strncmp(content_type, OGS_SBI_CONTENT_PATCH_TYPE,
                    strlen(OGS_SBI_CONTENT_PATCH_TYPE))) {
        if (item) {
            OpenAPI_patch_item_t *patch_item = NULL;
            cJSON *patchJSON = NULL;
            message->PatchItemList = OpenAPI_list_create();
            cJSON_ArrayForEach(patchJSON, item) {
                if (!cJSON_IsObject(patchJSON)) {
                    rv = OGS_ERROR;
                    ogs_error("Unknown JSON");
                    goto cleanup;
                }

                patch_item = OpenAPI_patch_item_parseFromJSON(patchJSON);
                OpenAPI_list_add(message->PatchItemList, patch_item);
            }
        }
    } else {
        SWITCH(message->h.service.name)
        CASE(OGS_SBI_SERVICE_NAME_NNRF_NFM)

            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_INSTANCES)
                message->NFProfile =
                    OpenAPI_nf_profile_parseFromJSON(item);
                if (!message->NFProfile) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SUBSCRIPTIONS)
                message->SubscriptionData =
                    OpenAPI_subscription_data_parseFromJSON(item);
                if (!message->SubscriptionData) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_NF_STATUS_NOTIFY)
                message->NotificationData =
                    OpenAPI_notification_data_parseFromJSON(item);
                if (!message->NotificationData) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NNRF_DISC)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NF_INSTANCES)
                message->SearchResult =
                    OpenAPI_search_result_parseFromJSON(item);
                if (!message->SearchResult) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAUSF_AUTH)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_UE_AUTHENTICATIONS)
                SWITCH(message->h.method)
                CASE(OGS_SBI_HTTP_METHOD_POST)
                    if (message->res_status == 0) {
                        message->AuthenticationInfo =
                            OpenAPI_authentication_info_parseFromJSON(item);
                        if (!message->AuthenticationInfo) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                            OGS_SBI_HTTP_STATUS_CREATED) {
                        message->UeAuthenticationCtx =
                        OpenAPI_ue_authentication_ctx_parseFromJSON(item);
                        if (!message->UeAuthenticationCtx) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                    break;
                CASE(OGS_SBI_HTTP_METHOD_PUT)
                    if (message->res_status == 0) {
                        message->ConfirmationData =
                            OpenAPI_confirmation_data_parseFromJSON(item);
                        if (!message->ConfirmationData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                        message->ConfirmationDataResponse =
                            OpenAPI_confirmation_data_response_parseFromJSON(
                                    item);
                        if (!message->ConfirmationDataResponse) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                    break;
                DEFAULT
                    rv = OGS_ERROR;
                    ogs_error("Unknown method [%s]", message->h.method);
                END
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NUDM_UEAU)
            SWITCH(message->h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_SECURITY_INFORMATION)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_GENERATE_AUTH_DATA)
                    if (message->res_status == 0) {
                        message->AuthenticationInfoRequest =
                        OpenAPI_authentication_info_request_parseFromJSON(
                                item);
                        if (!message->AuthenticationInfoRequest) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                        message->AuthenticationInfoResult =
                        OpenAPI_authentication_info_result_parseFromJSON(
                                item);
                        if (!message->AuthenticationInfoResult) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                    break;
                DEFAULT
                    rv = OGS_ERROR;
                    ogs_error("Unknown resource name [%s]",
                            message->h.resource.component[2]);
                END
                break;

            CASE(OGS_SBI_RESOURCE_NAME_AUTH_EVENTS)
                message->AuthEvent = OpenAPI_auth_event_parseFromJSON(item);
                if (!message->AuthEvent) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[1]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NUDM_UECM)
            SWITCH(message->h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_REGISTRATIONS)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_AMF_3GPP_ACCESS)
                    message->Amf3GppAccessRegistration =
                        OpenAPI_amf3_gpp_access_registration_parseFromJSON(
                                item);
                    if (!message->Amf3GppAccessRegistration) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                    break;
                DEFAULT
                    rv = OGS_ERROR;
                    ogs_error("Unknown resource name [%s]",
                            message->h.resource.component[2]);
                END
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[1]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NUDM_SDM)
            SWITCH(message->h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_AM_DATA)
                message->AccessAndMobilitySubscriptionData =
                    OpenAPI_access_and_mobility_subscription_data_parseFromJSON(
                            item);
                if (!message->AccessAndMobilitySubscriptionData) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SMF_SELECT_DATA)
                message->SmfSelectionSubscriptionData =
                    OpenAPI_smf_selection_subscription_data_parseFromJSON(item);
                if (!message->SmfSelectionSubscriptionData) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXT_IN_SMF_DATA)
                message->UeContextInSmfData =
                    OpenAPI_ue_context_in_smf_data_parseFromJSON(item);
                if (!message->UeContextInSmfData) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SM_DATA)
                message->SessionManagementSubscriptionData =
                    OpenAPI_session_management_subscription_data_parseFromJSON(
                            item);
                if (!message->SessionManagementSubscriptionData) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[1]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NUDR_DR)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_SUBSCRIPTION_DATA)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_AUTHENTICATION_DATA)
                    SWITCH(message->h.resource.component[3])
                    CASE(OGS_SBI_RESOURCE_NAME_AUTHENTICATION_SUBSCRIPTION)
                        if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                            message->AuthenticationSubscription =
                                OpenAPI_authentication_subscription_parseFromJSON(item);
                            if (!message->AuthenticationSubscription) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                        }
                        break;
                    CASE(OGS_SBI_RESOURCE_NAME_AUTHENTICATION_STATUS)
                        message->AuthEvent =
                            OpenAPI_auth_event_parseFromJSON(item);
                        if (!message->AuthEvent) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                        break;
                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown resource name [%s]",
                                message->h.resource.component[3]);
                    END
                    break;

                CASE(OGS_SBI_RESOURCE_NAME_CONTEXT_DATA)
                    message->Amf3GppAccessRegistration =
                        OpenAPI_amf3_gpp_access_registration_parseFromJSON(
                                item);
                    if (!message->Amf3GppAccessRegistration) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                    break;

                DEFAULT
                    SWITCH(message->h.resource.component[3])
                    CASE(OGS_SBI_RESOURCE_NAME_PROVISIONED_DATA)
                        SWITCH(message->h.resource.component[4])
                        CASE(OGS_SBI_RESOURCE_NAME_AM_DATA)
                            message->AccessAndMobilitySubscriptionData =
                                OpenAPI_access_and_mobility_subscription_data_parseFromJSON(item);
                            if (!message->AccessAndMobilitySubscriptionData) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                            break;

                        CASE(OGS_SBI_RESOURCE_NAME_SMF_SELECTION_SUBSCRIPTION_DATA)
                            message->SmfSelectionSubscriptionData =
                                OpenAPI_smf_selection_subscription_data_parseFromJSON(item);
                            if (!message->SmfSelectionSubscriptionData) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                            break;

                        CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXT_IN_SMF_DATA)
                            message->UeContextInSmfData =
                                OpenAPI_ue_context_in_smf_data_parseFromJSON(
                                        item);
                            if (!message->UeContextInSmfData) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                            break;

                        CASE(OGS_SBI_RESOURCE_NAME_SM_DATA)
                            message->SessionManagementSubscriptionData =
                                OpenAPI_session_management_subscription_data_parseFromJSON(item);
                            if (!message->SessionManagementSubscriptionData) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                            break;

                        DEFAULT
                            rv = OGS_ERROR;
                            ogs_error("Unknown resource name [%s]",
                                    message->h.resource.component[4]);
                        END
                        break;

                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown resource name [%s]",
                                message->h.resource.component[3]);
                    END
                END
                break;

            CASE(OGS_SBI_RESOURCE_NAME_POLICY_DATA)
                SWITCH(message->h.resource.component[1])
                CASE(OGS_SBI_RESOURCE_NAME_UES)
                    SWITCH(message->h.resource.component[3])
                    CASE(OGS_SBI_RESOURCE_NAME_AM_DATA)

                        message->AmPolicyData =
                            OpenAPI_am_policy_data_parseFromJSON(item);
                        if (!message->AmPolicyData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                        break;

                    CASE(OGS_SBI_RESOURCE_NAME_SM_DATA)

                        message->SmPolicyData =
                            OpenAPI_sm_policy_data_parseFromJSON(item);
                        if (!message->SmPolicyData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                        break;

                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown resource name [%s]",
                                message->h.resource.component[3]);
                    END
                    break;

                DEFAULT
                    rv = OGS_ERROR;
                    ogs_error("Unknown resource name [%s]",
                            message->h.resource.component[1]);
                END
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NSMF_PDUSESSION)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_SM_CONTEXTS)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_MODIFY)
                    if (message->res_status == 0) {
                        message->SmContextUpdateData =
                            OpenAPI_sm_context_update_data_parseFromJSON(item);
                        if (!message->SmContextUpdateData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                        message->SmContextUpdatedData =
                            OpenAPI_sm_context_updated_data_parseFromJSON(item);
                        if (!message->SmContextUpdatedData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_FORBIDDEN ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_NOT_FOUND ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_SERVICE_UNAVAILABLE ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_GATEWAY_TIMEOUT) {
                        message->SmContextUpdateError =
                            OpenAPI_sm_context_update_error_parseFromJSON(item);
                        if (!message->SmContextUpdateError) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                    break;
                CASE(OGS_SBI_RESOURCE_NAME_RELEASE)
                    if (message->res_status == 0) {
                        message->SmContextReleaseData =
                            OpenAPI_sm_context_release_data_parseFromJSON(item);
                        if (!message->SmContextReleaseData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                            OGS_SBI_HTTP_STATUS_NO_CONTENT) {
                    } else if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                        message->SmContextReleasedData =
                            OpenAPI_sm_context_released_data_parseFromJSON(
                                    item);
                        if (!message->SmContextReleasedData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                    break;
                DEFAULT
                    if (message->res_status == 0) {
                        message->SmContextCreateData =
                            OpenAPI_sm_context_create_data_parseFromJSON(item);
                        if (!message->SmContextCreateData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                            OGS_SBI_HTTP_STATUS_CREATED) {
                        message->SmContextCreatedData =
                            OpenAPI_sm_context_created_data_parseFromJSON(item);
                        if (!message->SmContextCreatedData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                            OGS_SBI_HTTP_STATUS_BAD_REQUEST ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_FORBIDDEN ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_NOT_FOUND ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_INTERNAL_SERVER_ERROR ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_SERVICE_UNAVAILABLE ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_GATEWAY_TIMEOUT) {
                        message->SmContextCreateError =
                            OpenAPI_sm_context_create_error_parseFromJSON(item);
                        if (!message->SmContextCreateError) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                END
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAMF_COMM)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_UE_CONTEXTS)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_N1_N2_MESSAGES)
                    if (message->res_status == 0) {
                        message->N1N2MessageTransferReqData =
                            OpenAPI_n1_n2_message_transfer_req_data_parseFromJSON(item);
                        if (!message->N1N2MessageTransferReqData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                                OGS_SBI_HTTP_STATUS_OK ||
                                message->res_status ==
                                    OGS_SBI_HTTP_STATUS_ACCEPTED) {
                        message->N1N2MessageTransferRspData =
                            OpenAPI_n1_n2_message_transfer_rsp_data_parseFromJSON(item);
                        if (!message->N1N2MessageTransferRspData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                    break;

                DEFAULT
                    rv = OGS_ERROR;
                    ogs_error("Unknown resource name [%s]",
                            message->h.resource.component[2]);
                END
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;
        CASE(OGS_SBI_SERVICE_NAME_NPCF_AM_POLICY_CONTROL)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_POLICIES)
                if (message->res_status == 0) {
                    message->PolicyAssociationRequest =
                        OpenAPI_policy_association_request_parseFromJSON(
                                item);
                    if (!message->PolicyAssociationRequest) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                } else if (message->res_status == OGS_SBI_HTTP_STATUS_CREATED) {
                    message->PolicyAssociation =
                        OpenAPI_policy_association_parseFromJSON(item);
                    if (!message->PolicyAssociation) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NPCF_SMPOLICYCONTROL)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_SM_POLICIES)
                if (!message->h.resource.component[1]) {
                    if (message->res_status == 0) {
                        message->SmPolicyContextData =
                            OpenAPI_sm_policy_context_data_parseFromJSON(item);
                        if (!message->SmPolicyContextData) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    } else if (message->res_status ==
                            OGS_SBI_HTTP_STATUS_CREATED) {
                        message->SmPolicyDecision =
                            OpenAPI_sm_policy_decision_parseFromJSON(item);
                        if (!message->SmPolicyDecision) {
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        }
                    }
                } else {
                    SWITCH(message->h.resource.component[2])
                    CASE(OGS_SBI_RESOURCE_NAME_DELETE)
                        if (message->res_status == 0) {
                            message->SmPolicyDeleteData =
                                OpenAPI_sm_policy_delete_data_parseFromJSON(
                                        item);
                            if (!message->SmPolicyDeleteData) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                        }
                        break;
                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown resource name [%s]",
                                message->h.resource.component[2]);
                    END
                    break;
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NNSSF_NSSELECTION)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_NETWORK_SLICE_INFORMATION)
                if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                    message->AuthorizedNetworkSliceInfo =
                        OpenAPI_authorized_network_slice_info_parseFromJSON(
                                item);
                    if (!message->AuthorizedNetworkSliceInfo) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NBSF_MANAGEMENT)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_PCF_BINDINGS)
                if (message->h.resource.component[1]) {
                    SWITCH(message->h.method)
                    CASE(OGS_SBI_HTTP_METHOD_PATCH)
                        if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                            message->PcfBinding =
                                OpenAPI_pcf_binding_parseFromJSON(item);
                            if (!message->PcfBinding) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                        }
                        break;
                    CASE(OGS_SBI_HTTP_METHOD_DELETE)
                        break;
                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown method [%s]", message->h.method);
                    END
                    break;
                } else {
                    SWITCH(message->h.method)
                    CASE(OGS_SBI_HTTP_METHOD_POST)
                        if (message->res_status == 0 ||
                            message->res_status ==
                                OGS_SBI_HTTP_STATUS_CREATED) {
                            message->PcfBinding =
                                OpenAPI_pcf_binding_parseFromJSON(item);
                            if (!message->PcfBinding) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                        }
                        break;

                    CASE(OGS_SBI_HTTP_METHOD_GET)
                        if (message->res_status == OGS_SBI_HTTP_STATUS_OK) {
                            message->PcfBinding =
                                OpenAPI_pcf_binding_parseFromJSON(item);
                            if (!message->PcfBinding) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                        }
                        break;
                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown method [%s]", message->h.method);
                    END
                    break;
                }

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NPCF_POLICYAUTHORIZATION)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_APP_SESSIONS)
                if (message->h.resource.component[1]) {
                    if (message->h.resource.component[2]) {
                        SWITCH(message->h.resource.component[2])
                        CASE(OGS_SBI_RESOURCE_NAME_DELETE)
                            /* Nothing */
                            break;
                        DEFAULT
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        END
                    } else {
                        SWITCH(message->h.method)
                        CASE(OGS_SBI_HTTP_METHOD_PATCH)
                            message->AppSessionContextUpdateDataPatch =
                                OpenAPI_app_session_context_update_data_patch_parseFromJSON(item);
                            if (!message->AppSessionContextUpdateDataPatch) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                            break;
                        DEFAULT
                            rv = OGS_ERROR;
                            ogs_error("JSON parse error");
                        END
                    }
                } else {
                    SWITCH(message->h.method)
                    CASE(OGS_SBI_HTTP_METHOD_POST)
                        if (message->res_status == 0 ||
                            message->res_status ==
                                OGS_SBI_HTTP_STATUS_CREATED) {
                            message->AppSessionContext =
                                OpenAPI_app_session_context_parseFromJSON(item);
                            if (!message->AppSessionContext) {
                                rv = OGS_ERROR;
                                ogs_error("JSON parse error");
                            }
                        }
                        break;
                    DEFAULT
                        rv = OGS_ERROR;
                        ogs_error("Unknown method [%s]", message->h.method);
                    END
                }
                break;
            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NAMF_CALLBACK)
            SWITCH(message->h.resource.component[1])
            CASE(OGS_SBI_RESOURCE_NAME_SM_CONTEXT_STATUS)
                message->SmContextStatusNotification =
                    OpenAPI_sm_context_status_notification_parseFromJSON(item);
                if (!message->SmContextStatusNotification) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[1]);
            END
            break;

        CASE(OGS_SBI_SERVICE_NAME_NSMF_CALLBACK)
            SWITCH(message->h.resource.component[0])
            CASE(OGS_SBI_RESOURCE_NAME_N1_N2_FAILURE_NOTIFY)
                message->N1N2MsgTxfrFailureNotification =
                    OpenAPI_n1_n2_msg_txfr_failure_notification_parseFromJSON(
                            item);
                if (!message->N1N2MsgTxfrFailureNotification) {
                    rv = OGS_ERROR;
                    ogs_error("JSON parse error");
                }
                break;

            CASE(OGS_SBI_RESOURCE_NAME_SM_POLICY_NOTIFY)
                SWITCH(message->h.resource.component[2])
                CASE(OGS_SBI_RESOURCE_NAME_UPDATE)
                    message->SmPolicyNotification =
                        OpenAPI_sm_policy_notification_parseFromJSON(item);
                    if (!message->SmPolicyNotification) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                    break;
                CASE(OGS_SBI_RESOURCE_NAME_TERMINATE)
                    message->TerminationNotification =
                        OpenAPI_termination_notification_parseFromJSON(item);
                    if (!message->TerminationNotification) {
                        rv = OGS_ERROR;
                        ogs_error("JSON parse error");
                    }
                    break;

                DEFAULT
                    rv = OGS_ERROR;
                    ogs_error("Unknown resource name [%s]",
                            message->h.resource.component[2]);
                END
                break;

            DEFAULT
                rv = OGS_ERROR;
                ogs_error("Unknown resource name [%s]",
                        message->h.resource.component[0]);
            END
            break;

        DEFAULT
            rv = OGS_ERROR;
            ogs_error("Not implemented API name [%s]",
                    message->h.service.name);
        END
    }

cleanup:

    cJSON_Delete(item);
    return rv;
}